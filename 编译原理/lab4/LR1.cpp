#include "LR1.h"

using namespace std;

//unordered_map<string, vector<vector<string>>> langMp;//�洢�ķ�����ʽ���£�<������,�����Ҳ����ÿһ������<ÿһ����Ĺ���ľ���ϸ��>
//vector<string> lgName; //���з��ս�����ķ�����
//unordered_map<string, unordered_set<string>> firstSet; //ÿ�����ս����first����
//unordered_map<string, unordered_set<string>> followSet;//ÿ�����ս����follow����
//unordered_map<int, unordered_set<Item>> LR0DfaStates;//ÿ��״̬��Ӧ����Ŀ��
//unordered_map<int, unordered_map<string, int>>LR0TransTable; //״̬ת�Ʊ�ÿ��״̬ͨ����ͬ������������Ӧ��һ��״̬


void LR1::init()
{
	langMp.clear(); //�洢ÿһ���ķ�
	lgName.clear(); //���з��ս�����ķ�����
	firstSet.clear(); //ÿ�����ս����first����
	followSet.clear(); //ÿ�����ս����follow����
	LR0StateID = 0; //LR0�����״̬���
}

void LR1::analysis()
{
	string ss;
	bool flag = true; //��������Ƿ��ǵ�һ���ķ�

	while (getline(cin, ss)) {
		if (ss.size() <= 0) break;
		string lrname = splitLang(ss);
		if (flag) {
			flag = false;
			this->startLang = lrname; //�洢�ķ���ʼ����
		}
		lgName.push_back(lrname); //�洢���з��ս��
	}
	//getline(cin, ss);
	//splitLang(ss);
	//printLang();
	getFirstSet();
	getFollowSet();
	//printLang();
	bool ok = judgeDieFirst();
	if (ok) {
		cout << "�������ݹ�" << endl;
		return ;
	}
	//���ж��Ƿ���Ҫ�޸��ķ���ʼ����
	newStart();


	DfaLR0();

	bool isSR = IsSLR1();
	if (!isSR) cout << "��SLR1�ķ�" << endl;

	return ;
}

//�ֽ��ķ�
string LR1::splitLang(string lang) {
	vector<string> tmp;
	int idx = 0;
	string lrgName = "";
	string rrgName = "";
	for (int i = 0; i < lang.size(); i++) {
		if (lang[i] == '-') {
			idx = i + 2;//ֱ��������һ������
			break;
		}
		if (isspace(lang[i])) continue;
		lrgName += lang[i];
	}

	//�ָ��Ҳ�
	for (int i = idx; i < lang.size(); i++) {
		//˵������һ���ַ�����һ�����С�����: A | B ����пո�
		if (isspace(lang[i]) && rrgName.size() > 0) {
			//����
			tmp.push_back(rrgName);
			rrgName.clear();
			continue;
		}
		// ˵���Ǳ����ķ���һ�����򣬴��ᴦ�� ��Ҫ�޸ġ�
		if (lang[i] == '|') {
			if (rrgName.size() > 0) {
				tmp.push_back(rrgName);
				rrgName.clear();
			}
			langMp[lrgName].push_back(tmp);
			tmp.clear();
			continue;
		}
		if (!isspace(lang[i])) rrgName += lang[i];
		if (i == lang.size() - 1) {
			if (rrgName.size() > 0) {
				tmp.push_back(rrgName);
				rrgName.clear();
			}
			langMp[lrgName].push_back(tmp);
		}
	}
	return lrgName;
}

//��first����
void LR1::getFirstSet()
{
	bool changed = true; //���first�����Ƿ��б仯
	while (changed) {
		changed = false;
		for (const auto & name: lgName) { //����ÿһ���ķ�
			const auto& gram = langMp[name];
			string gramName = name; //��ȡ���ķ�����
			int befSize = firstSet[gramName].size(); //�Ȼ�ȡ���ķ��󲿵�first������ǰԪ�ظ���
			for (const auto& line : gram) { //�������ķ����Ҳ��� ��(|) ��ÿһ������
				int isEmp = 0; //�ж����ù����Ƿ��п�
				for (int i = 0; i < line.size(); i++) {  //��������ÿһ���ӹ����µ�ÿһ������
					const auto& reg = line[i]; //�ַ�
					
					if (langMp.find(reg) == langMp.end()) { //˵�����ս����
						firstSet[gramName].insert(reg);
						if(firstSet[gramName].size() != befSize) //�����������ǰ��Ԫ�ظ�����˵���¼����Ԫ�ظ���
							changed = true;
						break;
					}
					else { //˵���Ƿ��ս����
						unordered_set<string> tmpSet = firstSet[reg]; //��ȡ�÷��ս���ŵ�first����
						if (tmpSet.find("@") != tmpSet.end()) { //����÷��ս���Ű�����
							//����������һ�����ս��������ABc,c���ս�������ǣ�ABC,C�Ƿ��ս��
							tmpSet.erase("@"); //�޳��գ���������һ�����ս�������޳���
							firstSet[gramName].insert(tmpSet.begin(), tmpSet.end());
							isEmp += 1; //���շ��ս���� + 1
							if (befSize != firstSet[gramName].size()) //˵��Ԫ���б䶯
								changed = true;
							continue; //���ս���Ұ����գ���������������������һ���ַ�
						}
						//����÷��ս���ǲ������յ�@
						firstSet[gramName].insert(tmpSet.begin(), tmpSet.end());
						if (firstSet[gramName].size() != befSize)
							changed = true;
						break; //�ǿ������
					}
				}
				//����ù�������ķ��ս��ַ������գ�������
				if (isEmp == line.size())
					firstSet[gramName].insert("@");
			}
		}
	}
	
}

//�ж�first�����Ƿ�������ݹ�
bool LR1::judgeDieFirst(){
	for (const auto& value : firstSet) {
		if (value.second.size() == 0) //ȷʵ�����ݹ顣
			return true;
	}
	return false;
}

//��follow����
void LR1::getFollowSet(){
	//�ȳ�ʼ����ʼ����
	followSet[startLang].insert("$");
	bool changed = true; //���follow�����Ƿ��б仯
	while (changed) {
		changed = false;
		for (const auto& notFinal : lgName) { //����ÿһ���ķ�
			const auto& gram = langMp[notFinal]; //��ȡ���ս�����ķ�
			for (const auto& line : gram) {//��ȡÿһ������
				for (int i = 0; i < line.size(); i++) {
					const auto& reg = line[i]; //��ȡÿһ���ַ�
					if (langMp.find(reg) == langMp.end()) //˵��������ս����follow��������
						continue;

					int befSize = followSet[reg].size(); //��ȡԭ�ȵ�follow���ϴ�С
					//����������ս����follow����
					int nextIdx = i + 1;
					//��ǰҪ��follow�ķ��ս�����Ѿ������һ��
					if (nextIdx == line.size()) { //����÷��ս��ַ������һ���������follow(notFinal)�����ķ��󲿵�follow()
						followSet[reg].insert(followSet[notFinal].begin(), followSet[notFinal].end());
						if (befSize != followSet[reg].size()) //����Ԫ���б仯
							changed = true;
						break; 
					}
					string nextReg; //��һ���ַ�
					//��һ���Ƿ��ս��ַ�
					for (; nextIdx < line.size(); nextIdx++) {
						nextReg = line[nextIdx];
						//��һ�����ս��ַ�
						if (firstSet.find(nextReg) == firstSet.end()) { 
							followSet[reg].insert(nextReg);
							if (befSize != followSet[reg].size()) //���follow���ϴ�С�б仯
								changed = true;
							break;
						}
						//��һ���Ƿ��ս��ַ�
						if (firstSet.find(nextReg) != firstSet.end()) { 
							const auto& nowFtSet = firstSet[nextReg];
							if(nowFtSet.find("@") == nowFtSet.end()) { //˵����һ���ַ�first�������ڿգ��洢������Ѱ��
								followSet[reg].insert(nowFtSet.begin(), nowFtSet.end()); //�������first����
								if (befSize != followSet[reg].size()) //�����С�����ı�
									changed = true;
								break;
							}
							//������ڿգ�������һ����
							followSet[reg].insert(nowFtSet.begin(), nowFtSet.end());
							followSet[reg].erase("@");
							if (nextIdx == line.size() - 1) { //�Ѿ������һ�����ս���ţ���Ҫ����
								followSet[reg].insert(followSet[notFinal].begin(), followSet[notFinal].end());
								if (befSize != followSet[reg].size()) //�����С�����ı�
									changed = true;
								break;
							}
						}
					}
				}
			}
		}
	}
}

//�ж��ķ���ʼ�����Ƿ���Ҫ����
void LR1::newStart()
{
	if (langMp[startLang].size() > 1) {
		string nStart = startLang + "'";
		vector<string> tmp = { startLang };
		langMp[nStart].push_back(tmp);
		firstSet[nStart].insert(firstSet[startLang].begin(), firstSet[startLang].end());
		followSet[nStart].insert("$");
		startLang = nStart;
	}
}

/*-------------------------------- - �ж��Ƿ�ΪLR0-----------------------------------------*/
//����ÿ����Ŀ���Ĺ���հ��������Ƶ����������Ŀ��
void LR1::getClosureLR0(unordered_set<ItemLR0>& items)
{
	bool changed = true;
	while (changed) {
		changed = false;

		unordered_set<ItemLR0> newItems = items; // �����¼��ϣ���ŵ�ǰ�հ�״̬

		for (const auto& item : items) {
			if (item.dotPos < item.right.size()) { //�����Ƿ���ĩβ
				string symbol = item.right[item.dotPos]; //���ķ���
				if (langMp.find(symbol) != langMp.end()) { //����Ƿ��ս����
					for (const auto& rule : langMp[symbol]) { //�����÷��ŵ�ÿһ������
						ItemLR0 newItem = { symbol,rule, 0};// ��������Ŀ����λ���ڿ�ͷ
						if (newItems.find(newItem) == newItems.end()) { //�����״̬���޸���Ŀ��������
							newItems.insert(newItem);
							changed = true;
						}
					}
				}
			}
		}
		items.insert(newItems.begin(), newItems.end()); //���±հ�״̬
	}
}

// GOTOLR0 ��������״̬�������������ƽ�������״̬
//��ÿ��״̬���з����ƽ��Ĳ�����������״̬��������״̬�ϲ�
unordered_set<ItemLR0> LR1::GotoLR0(const unordered_set<ItemLR0>& state, const string& symbol) {
	if (symbol == "@") { //ǰ���ж������⣬���룬�򵯻�ȥ
		cout << "GotoLR0������@��" << endl;
		return {};
	}
	unordered_set<ItemLR0>nextState; //��һ��״̬��

	//����Ŀǰ״̬�����ƽ�
	for (const auto& item : state) {
		if (item.dotPos < item.right.size() && item.right[item.dotPos] == symbol) { //�ж��ǲ���Ŀ����һ��Ҫ�ƶ��ķ���
			//�ƶ����λ��
			ItemLR0 movedItem = item;
			movedItem.dotPos++;
			nextState.insert(movedItem);
		}
	}

	//����հ�
	getClosureLR0(nextState);

	return nextState;
}

//��LR0��DFAͼ
void LR1::DfaLR0()
{
	//��ʼ��״̬����
	unordered_set<ItemLR0> startState; //��״̬����Ŀ��
	startState.insert({ startLang, langMp[startLang][0], 0 }); // �����ʼ��Ŀ�����󲿷���, �Ҳ�����, ��λ�ã�
	getClosureLR0(startState); //�Ը�״̬��Ŀ������հ��������һ�����ս��ַ���������Ŀ��
	int stateId = 0; //״̬ID

	
	queue<pair<unordered_set<ItemLR0>, int>> stateQueue; //�洢״̬����
	stateQueue.push({ startState ,stateId });
	LR0DfaStates[stateId] = startState ;//��ÿ��״̬��״̬���ϴ洢��dfaState+LR0TransTable�պ���ɣ�ÿ��״̬����Ŀ�� + ��״̬����Ŀ��ͨ������k�ִ���һ��״̬���


	unordered_map<unordered_set<ItemLR0>, int, hash<unordered_set<ItemLR0>>> StateToId; //�����ж��Ƿ����µ�״̬����
	StateToId[startState] = stateId++; //��ʼ״̬����

	while (!stateQueue.empty()) {
		pair<unordered_set<ItemLR0>, int> tmp = stateQueue.front();

		unordered_set<ItemLR0> currentState = tmp.first; //��ǰ״̬��
		int currentId = tmp.second; //��ǰ״̬���

		stateQueue.pop();

		//�ռ���ǰ��Ŀ���ڸ�LRλ��(.)��һ���ǿ��ƽ��ķ��ż��ϡ�
		unordered_set<string> symbols;
		for (const auto& item : currentState) {
			if (item.right.size() == 1&& item.right[0] == "@") continue; //��@
			if (item.dotPos < item.right.size()) {
				symbols.insert(item.right[item.dotPos]);
			}
		}

		//��ʼ��ÿ�����ƽ����Ž��и�״̬��Ŀ���ϵı�������ǰ��һ���ƽ�����Ϊsymbolʱ������״̬��
		for (const auto& symbol : symbols) {
			unordered_set<ItemLR0> nextState = GotoLR0(currentState, symbol); //ͨ��gotolr0���������һ��״̬����Ŀ��

			if (nextState.empty()) continue; //�޷��ƽ�����������һ��״̬

			//�ж��Ƿ�Ϊ�µ�״̬����
			if (StateToId.find(nextState) == StateToId.end()) {
				StateToId[nextState] = stateId; //����״̬��¼
				LR0DfaStates[stateId] = nextState; //״̬���¼
				LR0TransTable[currentId][symbol] = stateId; //����״̬ת�Ʊ�
				stateQueue.push({ nextState, stateId });
				stateId++;
			}
			else { //�����¼���ҲҪ����״̬ת����
				if(LR0TransTable[currentId].find(symbol) == LR0TransTable[currentId].end()) //��֤���ظ�idת�Ƴ���
					LR0TransTable[currentId][symbol] = StateToId[nextState]; //����״̬ת�Ʊ�
			}
		}
	}
	LR0StateID = stateId - 1;
}

/*-------------------------------- - �ж��Ƿ�ΪSLR1-----------------------------------------*/
//�ж��Ƿ�ΪSLR1�ķ�
//���ģ��ж�follow�����Ƿ����ƽ�-��Լ��ͻ����Լ-��Լ��ͻ��
bool LR1::IsSLR1()
{
	bool isSlr1 = true; //�ٶ���slr1�ķ�

	//unordered_map<int, unordered_set<Item>> LR0DfaStates;
	for (const auto& state : LR0DfaStates) { //����ÿһ��״̬����Ŀ����
		int stateId = state.first;
		unordered_set<ItemLR0> items = state.second; //��ǰ��Ŀ״̬��

		unordered_set<string> finalFolSet; //�洢��Լ��Ŀ�󲿶�Ӧ�� follow ����
		unordered_set<string> shiftFirstSet; //�洢�ƽ���Ŀ��Ӧ��first���ϡ���������н�����˵�����ƽ�-��Լ��ͻ
		
		//������ǰ״̬��ÿ����Ŀ
		for (const auto& item : items) {
			if(item.dotPos == item.right.size()) {//��Լ��
				if (item.right.size() == 1 && item.right[0] == "@") continue; //��Ȼ@��Ŀ��Զ����ִ��item.dotPos == size()����Ϊû���ƽ�

				const auto& follow = followSet[item.left];
				
				// �������Լ��� Follow ���Ƿ���ڳ�ͻ
				for (const auto& symbol : follow) {
					if (symbol == "@") continue;
					if (finalFolSet.count(symbol)) { // �ǿշ��ŵĳ�ͻ���
						cout << "��Լ-��Լ��ͻ��״̬ " << stateId << " ���� " << symbol << endl;
						isSlr1 = false;
					}
					finalFolSet.insert(symbol); // ���� Follow ��,follow���϶����ս��������
				}
			}
			else { //�ƽ���
				string nextSymbol = item.right[item.dotPos];

				if (nextSymbol == "@") continue; //�ղ���

				if (firstSet.find(nextSymbol) != firstSet.end()) { //����Ƿ��ս����
					shiftFirstSet.insert(firstSet[nextSymbol].begin(), firstSet[nextSymbol].end());
				}
				else {
					shiftFirstSet.insert(nextSymbol);
				}
			}
		}

		//����ƽ�-��Լ��ͻ
		for (const auto& symbol : shiftFirstSet) {
			if (symbol != "@" && finalFolSet.count(symbol)) { //�н���
				cout << "��״̬:"<< stateId <<" ,�����ƽ�-��Լ��ͻ������Ϊ:" << symbol << endl;
				isSlr1 = false;
			}
		}

	}
	return isSlr1;
}


/*----------------------------------LR1����------------------------------------------------ - */
//����ÿ����Ŀ���Ĺ���հ��������Ƶ����������Ŀ����������LR1�ġ�������룬�޸�������lookahead
void LR1::getClosureLR1(unordered_set<ItemLR1>& items)
{
	bool changed = true;
	while (changed) {
		changed = false;

		unordered_set<ItemLR1> newItems = items; // �����¼��ϣ���ŵ�ǰ�հ�״̬

		//��ʼ���бհ���չ
		for (const auto& item : items) {
			if (item.dotPos < item.right.size()) { //�����Ƿ���ĩβ
				string symbol = item.right[item.dotPos]; //���ķ���
				unordered_set<string> ahead; //����÷��ŵ�follow���ϣ�����lookahead
				if (item.dotPos + 1 == item.right.size()) { //�պ��Ƿ���ĩβ�ˣ���̳�ԭ�ȵ�ahead
					ahead = item.lookahead;
				}
				else if (langMp.find(item.right[item.dotPos + 1]) != langMp.end()) { //�����һ���Ƿ��ս���ţ�������first����
					ahead.insert(item.lookahead.begin(), item.lookahead.end());
				}
				else { //�����һ�������ս����
					ahead.insert(item.right[item.dotPos + 1]);
				}

				if (langMp.find(symbol) != langMp.end()) { //�����һ�������Ƿ��ս���ţ�����Ҫ��հ�
					for (const auto& rule : langMp[symbol]) { //�����÷��ŵ�ÿһ������
						ItemLR1 newItem = { symbol,rule, 0 };// ��������Ŀ����λ���ڿ�ͷ
						newItem.lookahead = ahead; //��������ǰ�����lookahead��Ŀ
						if (newItems.find(newItem) == newItems.end()) { //�����״̬���޸���Ŀ��������
							newItems.insert(newItem);
							changed = true;
						}
					}
				}
			}
		}
		items.insert(newItems.begin(), newItems.end()); //���±հ�״̬
	}
}

//������LR1�ģ���ÿ��״̬���з����ƽ��Ĳ�����������״̬��������״̬�ϲ����������û���κ��޸ĵĵط���������getClosureLR1��
unordered_set<ItemLR1> LR1::GotoLR1(const unordered_set<ItemLR1>& state, const string& symbol) {
	unordered_set<ItemLR1> nextState;

	for (const auto& item : state) {
		if (item.dotPos < item.right.size() && item.right[item.dotPos] == symbol) {
			ItemLR1 movedItem = item;
			movedItem.dotPos++;
			nextState.insert(movedItem);
		}
	}
	getClosureLR1(nextState);//���ƽ�����������հ�����չ����)����ͬʱ����lookahead��

	return nextState;
}

//����LR1��DFAͼ
void LR1::DfaLR1()
{
	unordered_set<ItemLR1> startState;
	startState.insert({ startLang, langMp[startLang][0], 0, {"$"} }); // ��ʼ��Ŀ����$ Ϊ lookahead
	getClosureLR1(startState); //������ʼ״̬�հ�

	int stateId = 0;
	queue<pair<unordered_set<ItemLR1>, int>> stateQueue; //�洢��<��Ŀ����״̬���>
	stateQueue.push({ startState, stateId });
	LR1DfaStates[stateId] = startState; //�洢LR1��״̬��Ŀ��

	unordered_map<unordered_set<ItemLR1>, int, hash<unordered_set<ItemLR1>>> stateToId; //�����ж��Ƿ����µ�״̬��Ŀ������
	stateToId[startState] = stateId++;

	//��ʼbfa�����ķ�������״̬
	while (!stateQueue.empty()) {
		const auto& tmp = stateQueue.front();
		unordered_set<ItemLR1> currentState = tmp.first; //��ǰ״̬��
		int currentId = tmp.second; //��ǰ״̬ID

		stateQueue.pop();

		unordered_set<string> symbols; //�ռ���ǰ��Ŀ���ڸ�LRλ��(.)��һ���ǿ��ƽ��ķ��ż��ϡ�
		for (const auto& item : currentState) {
			if (item.right.size() == 1 && item.right[0] == "@") continue; //��@
			if (item.dotPos < item.right.size()) {
				symbols.insert(item.right[item.dotPos]);
			}
		}

		//��ʼ�����ƽ���������ÿһ�ַ��Ž����ƽ���lookahead�����հ�ʱ������Ϊ����ԭ�ؽ�����չ���������š��ƽ������Ƿ���ĳ���������ƽ�
		for (const auto& symbol : symbols) {
			unordered_set<ItemLR1> nextState = GotoLR1(currentState, symbol); //���м��ƽ��������ڲ�������հ�����
			
			if (nextState.empty()) continue; //ͬ��ģ�����һ��Ԥ��������

			if (stateToId.find(nextState) == stateToId.end()) {//����״̬�������״̬��Ŀ��������״̬Id
				stateToId[nextState] = stateId;
				LR1DfaStates[stateId] = nextState; //�洢״̬��Ŀ
				LR1TransTable[currentId][symbol] = stateId; //��¼״̬��Ŀͨ��ʲô���Ž���ת�Ƶ���һ����Ŀ
				stateQueue.push({ nextState, stateId });
				stateId++;
			}
			else { //��ʹ�Ǿ�״̬��ҲӦ�ü���״̬ת������¼״̬��Ŀͨ��ʲô���Ž��У�ת�Ƶ��Ѵ��ڵ�״̬��
				LR1TransTable[currentId][symbol] = stateToId[nextState]; //��ǰ״̬��Ŀ����currentIdͨ��symbol����ת�Ƶ�������һ��״̬Id
			}
		}

	}

}


