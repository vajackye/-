#pragma once
#include<iostream>
#include<unordered_map>
#include<unordered_set>

#include<string>
#include<vector>
#include<queue>
#include<utility>

using namespace std;

//LR0��ÿһ������洢�ṹ
struct ItemLR0 {
	string left; //������
	vector<string> right; //�����Ҳ�
	int dotPos = 0; //���λ��

	//�Ƚ�����Item�Ƿ���ͬ
	bool operator==(const ItemLR0& other) const {
		return left == other.left && right == other.right && dotPos == other.dotPos;
	}
};

//ͼ���㣬ֱ��copy�����ˡ�ʵ�ʿ����в�����д�������
struct ItemLR1 {
	string left; //������
	vector<string> right; //�����Ҳ�
	int dotPos = 0; //���λ��
	unordered_set<string> lookahead; //LR1�ĳ�ǰ�鿴һ���ַ������ܴ��ڶ��������(|)

	//�Ƚ�����ItemLR1�Ƿ���ͬ
	bool operator==(const ItemLR1& other) const {
		return left == other.left && right == other.right && dotPos == other.dotPos && lookahead == other.lookahead;
	}
};

//���Ի�hash����֤�ܹ�����ItemLR0����
namespace std {
	// ���� Item ���͵Ĺ�ϣ����
	template <>
	struct hash<ItemLR0> {
		size_t operator()(const ItemLR0& item) const {
			size_t h1 = hash<string>()(item.left); // �� left �ֶι�ϣ
			size_t h2 = hash<size_t>()(item.dotPos); // �� dotPos �ֶι�ϣ
			size_t h3 = 0;
			for (const auto& s : item.right) {
				h3 ^= hash<string>()(s) + 0x9e3779b9 + (h3 << 6) + (h3 >> 2); // �ϲ���ϣ��Ϊright�ֶι�ϣֵ
			}
			return h1 ^ h2 ^ h3; // ������Ϲ�ϣֵ
		}
	};

	// ���� unordered_set<Item> �Ĺ�ϣ����
	template <>
	struct hash<unordered_set<ItemLR0>> {
		size_t operator()(const unordered_set<ItemLR0>& set) const {
			size_t hashValue = 0;
			for (const auto& item : set) {
				hashValue ^= hash<ItemLR0>()(item); // ��ÿ��Ԫ�صĹ�ϣֵ����������
			}
			return hashValue; // ���ؼ��ϵ���Ϲ�ϣֵ
		}
	};

	// ���� unordered_set<Item> �ıȽϺ���
	template <>
	struct equal_to<unordered_set<ItemLR0>> {
		bool operator()(const unordered_set<ItemLR0>& lhs, const unordered_set<ItemLR0>& rhs) const {
			return lhs == rhs; // ʹ�ü����Դ��������������бȽ�
		}
	};
}

//���Ի�hash����֤�ܹ�����ItemLR1����
namespace std {
	// Ϊ ItemLR1 ���Ͷ����ϣ����
	template <>
	struct hash<ItemLR1> {
		size_t operator()(const ItemLR1& item) const {
			size_t h1 = hash<string>()(item.left);  // �� left �ֶι�ϣ
			size_t h2 = hash<size_t>()(item.dotPos);  // �� dotPos �ֶι�ϣ
			size_t h3 = 0;

			// �� right �ֶε�ÿ��Ԫ�ؽ��й�ϣ
			for (const auto& s : item.right) {
				h3 ^= hash<string>()(s) + 0x9e3779b9 + (h3 << 6) + (h3 >> 2);  // �ϲ���ϣֵ
			}

			size_t h4 = 0;
			// �� lookahead �����е�ÿ��Ԫ�ؽ��й�ϣ
			for (const auto& lookaheadSymbol : item.lookahead) {
				h4 ^= hash<string>()(lookaheadSymbol) + 0x9e3779b9 + (h4 << 6) + (h4 >> 2);
			}

			return h1 ^ h2 ^ h3 ^ h4;  // �ϲ������ֶεĹ�ϣֵ
		}
	};

	// Ϊ unordered_set<ItemLR1> ���Ͷ����ϣ����
	template <>
	struct hash<unordered_set<ItemLR1>> {
		size_t operator()(const unordered_set<ItemLR1>& set) const {
			size_t hashValue = 0;
			for (const auto& item : set) {
				hashValue ^= hash<ItemLR1>()(item);  // ��ÿ�� ItemLR1 �Ĺ�ϣֵ����������
			}
			return hashValue;  // ���ؼ��ϵ���Ϲ�ϣֵ
		}
	};

	// Ϊ unordered_set<ItemLR1> ���Ͷ�����ȱȽϺ���
	template <>
	struct equal_to<unordered_set<ItemLR1>> {
		bool operator()(const unordered_set<ItemLR1>& lhs, const unordered_set<ItemLR1>& rhs) const {
			return lhs == rhs;  // ʹ�� unordered_set �Դ�����ȱȽ������
		}
	};
}

class LR1
{
public:
	unordered_map<string, vector<vector<string>>> langMp;//�洢�ķ�����ʽ���£�<������,�����Ҳ����ÿһ������<ÿһ����Ĺ���ľ���ϸ��>
	vector<string> lgName; //���з��ս�����ķ�����
	unordered_map<string, unordered_set<string>> firstSet; //ÿ�����ս����first����
	unordered_map<string, unordered_set<string>> followSet;//ÿ�����ս����follow����

	//LR0�Ĵ洢�ṹ
	//����LR0TransTable��������Ϊ����QT��չʾ��ϵ���ˡ����򶼲���Ҫ������洢�ṹ
	unordered_map<int, unordered_set<ItemLR0>> LR0DfaStates;//LR0ÿ��״̬��Ӧ����Ŀ��
	unordered_map<int, unordered_map<string, int>>LR0TransTable; //LR0״̬ת�Ʊ�ÿ��״̬ͨ����ͬ������������Ӧ��һ��״̬

	//LR1�Ĵ洢�ṹ
	unordered_map<int, unordered_set<ItemLR1>> LR1DfaStates; //LR1ÿ��״̬��Ӧ����Ŀ��
	unordered_map<int, unordered_map<string, int>>LR1TransTable; //LR1״̬ת�Ʊ�ÿ��״̬ͨ����ͬ������������Ӧ��һ��״̬


	string startLang;//�ķ���ʼ����,LR0��LR1����
	int LR0StateID; //LR0�������
	int LR1StateId; //LR1�������
	void init(); //��ʼ������
	void analysis(); //��������
	string splitLang(string lang); //�и��ķ�
	void getFirstSet(); //��first����
	bool judgeDieFirst(); //�ж��Ƿ�����������ݹ��
	void getFollowSet(); //��follow����
	void newStart(); //�ж��Ƿ���Ҫ�����ķ���ʼ����

	//LR0����
	void getClosureLR0(unordered_set<ItemLR0>& items); //����LR��DFAͼ����Ŀ�����ݹ�ؽ����п��ܵ���չ�������չ���ͷ��ţ���ӵ���Ŀ���С�
	unordered_set<ItemLR0> GotoLR0(const unordered_set<ItemLR0>& state, const string& symbol); //��LR0ÿ��״̬���з����ƽ��Ĳ�����������״̬��������״̬�ϲ���
	void DfaLR0(); //��ȡLR0��DFAͼ��
	bool IsSLR1(); //�ж��Ƿ�ΪSLR1����չʾ�жϽ������SLR1չ��ԭ��

	//LR1��Ӧ��Ҫ�������ࣺgetClosureLR��Goto�������Լ�Item�ṹ��͹�ϣ�Ƚϡ������޸Ŀ���Ӧ����
	void getClosureLR1(unordered_set<ItemLR1>& items); //LR1��DFA״̬����Ŀ�����ݹ���չ
	unordered_set<ItemLR1> GotoLR1(const unordered_set<ItemLR1>& state, const string& symbol); //��LR1ÿ��״̬���з����ƽ��Ĳ�����������״̬��������״̬�ϲ���
	void DfaLR1(); //��ȡLR1��DFAͼ��
	void AnalysisLR1(); //��ȡLR1�ķ�����
};

