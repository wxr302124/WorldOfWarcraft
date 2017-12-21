#include <iostream>
#include <cstring>
#include <iomanip>
#include <climits>
#include <string>
#include <cstdio>
#include <cstdlib>
using namespace std;

const int RED = 0, BLUE = 1;
const int DRAGON = 0, NINJA = 1, ICEMAN = 2, LION = 3, WOLF = 4;
const int SWORD = 0, BOMB = 1, ARROW = 2;
const string Color[2] = { "red", "blue" };
bool GameOver = false;
int hour = 0;//Сʱ
int minute = 0;//����
int M = 0, N = 0, R = 0, K = 0, T = 0;
//M:˾���ʼ����Ԫ
//N:N������
//R:arrow�Ĺ�����
//K:lionÿ����һ��δ��ɱ�����˵�ս�����ҳ϶Ⱦͽ���K
//T:�����0ʱ0�ֿ�ʼ����ʱ��TΪֹ(����T)�������¼����Է���Ϊ��λ��0 <= T <= 5000

//��ǰ����
class CHeadquarter;
class CCity;
class CWarrior;
class CWolf;

class CWeapon{
public:
	static const string Names[3];//��������
	int ID;//��ţ�0��1��2
	int ATK;
	int count_used;//��¼arrow��ʹ�ô���
	CWeapon(){
		count_used = 0;
	}
	virtual void Dulled(){}
};
class CSword :public CWeapon{//����  ����
public:
	CSword(int a){
		ID = SWORD;
		ATK = a;
	}
	void Dulled(){//��ۣ���������Ϊ����ս��ǰ��80% (ȥβȡ��)
		ATK *= 0.8;
	}
};
class CBomb :public CWeapon{//����  ը����
public:
	CBomb(){
		ATK = 0;
		ID = BOMB;
	}
};
class CArrow :public CWeapon{//����  ������
public:
	CArrow(){
		ID = ARROW;
		ATK = R;
	}
	void released();
};

class CHeadquarter{//˾�
public:
	int color;//����졢��˾���������ֵ��RED=0��BLUE=1
	int LifePoint;//ʣ������Ԫ
	int tmpLifePoint;//���ڽ���
	static int InitialLifePoint;//˾���ʼ����Ԫ
	static int MakingSeq[2][5];//ת����������ʿ��˳��
	int warr_num;//��ʿ����Ŀ
	CWarrior *pWarriors[1000];//ָ����������ʿ
	bool Win;
	void Init(int c, int M);
	void ProduceWarr();//������ʿ
	void WarrMove();
	void Reward();
	void ReportLP();
};
CHeadquarter red_headquarter, blue_headquarter;

class CCity{//����
	//�����ĳ��У��Լ����Ϊ������������У��ɺ���ʿ�������������
	//������ĳ��У��Լ����Ϊż����������У�������ʿ�����������
public:
	int ID;//���
	int flag;//RED��BLUE��-1Ϊ����
	int LifePoint;
	int record[1000];//��¼ÿ��ʤ����ƽ��Ϊ-1����ʤΪRED����ʤΪBLUE
	int battle_num;
	CWarrior *pWarriors[2];
	CCity(){//��ʼ��
		memset(record, -1, sizeof(record));//ʤ�������ʼȫΪƽ��
		battle_num = 0;
	}
	void Init(int id);
	void LionEscape();
	void PrintMove();
	void WarrGetLP1();//ȡ������Ԫ
	void ReleaseArrow();//��ʿ�ż�
	void UseBomb();
	void Battle();//һ��ս��
	void Attack();//��������ʿս��
	void Fightback();
	void Killed();
	void WarrGetLP2();//սʤ��ȡ������Ԫ
	void RecordVictory();//��ս���������¼ʤ�����
	void ChangeFlag();
	void DisposeBody();
	void redWarrReportWP();
	void blueWarrReportWP();
};
CCity *pCities = NULL;

class CWarrior{//��ʿ��
public:
	static const string Names[5];//������ʿ������
	static int InitialHP[5];//������ʿ��ʼ����ֵ
	static int InitialATK[5];//������ʿ������
	bool reach_headq;
	int warr_type;//��ʿ����
	int ID;//���
	int HP;//healthpoint ����ֵ
	int ATK;//attack ������
	CHeadquarter *pHeadquarter;//����˾�
	int color;
	int pC_ID;//��ǰ���ڳ��б��
	int steps;//���ߵĲ���
	CCity *pCity;//��ǰ���ڳ���
	CWarrior(CHeadquarter *p, int id){
		reach_headq = false;
		pHeadquarter = p;
		ID = id;
		color = pHeadquarter->color;
		if (color == RED){
			pC_ID = 0;
			pCity = &pCities[0];
			pCity->pWarriors[RED] = this;
		}
		else if (color == BLUE){
			pC_ID = N + 1;
			pCity = &pCities[N + 1];
			pCity->pWarriors[BLUE] = this;
		}
		steps = 0;
	};
	virtual ~CWarrior(){};
	virtual void print_IniAttr();
	virtual bool has_bomb(){
		return false;
	}
	virtual bool isDead(){
		if (HP <= 0)    return true;
		return false;
	}
	virtual void release_arrow() = 0;
	virtual void Attack(CWarrior *p){
		//������000:40 red iceman 1 attacked blue lion 1 in city 1 with 20 elements and force 30
		printf("%03d:%02d %s %s %d attacked %s %s %d in city %d with %d elements and force %d\n",
			hour, minute, Color[color].c_str(), Names[warr_type].c_str(), ID, Color[p->color].c_str(), Names[p->warr_type].c_str(), p->ID, pC_ID, HP, ATK);
	}
	virtual void Hurted(int power){
		HP -= power;
	}
	virtual void FightBack(CWarrior *p){
		//���������001:40 blue dragon 2 fought back against red lion 2 in city 1 
		printf("%03d:%02d %s %s %d fought back against %s %s %d in city %d\n",
			hour, minute, Color[color].c_str(), Names[warr_type].c_str(), ID, Color[p->color].c_str(), Names[p->warr_type].c_str(), p->ID, pC_ID);
	}
	virtual bool lion_escaped(){
		return false;
	}
	virtual void print_City();
	void move();//��ʿ�ƶ�����һ������
	virtual int get_wpATK(){//��ȡsword�Ĺ�����ֵ
		return 0;
	}
	virtual bool win(){
		if (pCity->pWarriors[1 - color] == NULL)    return false;
		if (!isDead() && pCity->pWarriors[1 - color]->isDead())    return true;
		return false;
	}
	virtual void change_morale(){}
	virtual void capture_weapons(CWarrior *p){}
	virtual void turn_in_weapons(CWolf *p){}
	virtual bool yell(){
		return false;
	}
	virtual void change_loyalty(){}
	virtual void print_Weapon() = 0;
};
class CDragon :public CWarrior{
public:
	CWeapon *pWeapon;
	double morale;
	CDragon(CHeadquarter *p, int id) :CWarrior(p, id){
		warr_type = DRAGON;
		pWeapon = NULL;
		HP = InitialHP[DRAGON];
		ATK = InitialATK[DRAGON];
		switch (ID % 3)
		{
		case SWORD:
			if (ATK * 0.2 >= 1){
				pWeapon = new CSword(ATK*0.2);
			}
			break;
		case BOMB:pWeapon = new CBomb; break;
		case ARROW:pWeapon = new CArrow; break;
		default:
			break;
		}
		morale = (double)(pHeadquarter->LifePoint) / (double)InitialHP[DRAGON];
	}
	~CDragon(){
		if (pWeapon)    delete pWeapon;
	}
	bool has_bomb(){
		if (pWeapon != NULL&&pWeapon->ID == BOMB)    return true;
		return false;
	}
	void Attack(CWarrior *p){
		if (isDead())    return;
		CWarrior::Attack(p);
		int wpATK = 0;
		if (pWeapon != NULL&&pWeapon->ID == SWORD){
			wpATK = pWeapon->ATK;
			pWeapon->Dulled();
			if (pWeapon->ATK == 0){
				delete pWeapon;
				pWeapon = NULL;
			}
		}
		p->Hurted(ATK + wpATK);
		p->FightBack(this);
	}
	void FightBack(CWarrior *p){
		if (isDead())    return;
		CWarrior::FightBack(p);
		int wpATK = 0;
		if (pWeapon != NULL&&pWeapon->ID == SWORD){
			wpATK = pWeapon->ATK;
			pWeapon->Dulled();
			if (pWeapon->ATK == 0){
				delete pWeapon;
				pWeapon = NULL;
			}
		}
		p->Hurted(ATK / 2 + wpATK);
	}
	void print_IniAttr(){
		CWarrior::print_IniAttr();
		printf("Its morale is %.2f\n", morale);
	}
	void print_City(){
		CWarrior::print_City();
	}
	void release_arrow();
	int get_wpATK(){
		int wpATK = 0;
		if (pWeapon != NULL&&pWeapon->ID == SWORD)    wpATK = pWeapon->ATK;
		return wpATK;
	}
	bool yell(){
		if (morale > 0.8&&HP > 0)    return true;
		return false;
	}
	void print_Weapon();
	void chang_morale(){
		if (win())    morale += 0.2;
		else    morale -= 0.2;
	}
	void turn_in_weapons(CWolf *p);
};
class CNinja :public CWarrior{
public:
	CWeapon *pWeapons[2];
	CNinja(CHeadquarter *p, int id) :CWarrior(p, id){
		warr_type = NINJA;
		HP = InitialHP[NINJA];
		ATK = InitialATK[NINJA];
		memset(pWeapons, NULL, sizeof(pWeapons));
		switch (ID % 3)
		{
		case SWORD:
			if (ATK*0.2 >= 1){
				pWeapons[0] = new CSword(ATK*0.2); 
			}
			break;
		case BOMB:pWeapons[0] = new CBomb; break;
		case ARROW:pWeapons[0] = new CArrow; break;
		default:
			break;
		}
		switch ((ID + 1) % 3)
		{
		case SWORD:
			if (ATK * 0.2 >= 1){
				pWeapons[1] = new CSword(ATK*0.2);
			}
			break;
		case BOMB:pWeapons[1] = new CBomb; break;
		case ARROW:pWeapons[1] = new CArrow; break;
		default:
			break;
		}
	}
	~CNinja(){
		for (int k = 0; k <= 1; ++k){
			if (pWeapons[k])    delete pWeapons[k];
		}
	}
	bool has_bomb(){
		for (int k = 0; k <= 1; ++k){
			if (pWeapons[k] != NULL&&pWeapons[k]->ID == BOMB)    return true;
		}
		return false;
	}
	void print_IniAttr(){
		CWarrior::print_IniAttr();
	}
	void print_City(){
		CWarrior::print_City();
	}
	void release_arrow();
	void Attack(CWarrior *p){
		if (isDead())    return;
		CWarrior::Attack(p);
		int wpATK = 0;
		for (int k = 0; k <= 1; ++k){
			if (pWeapons[k] != NULL&&pWeapons[k]->ID == SWORD){
				wpATK = pWeapons[k]->ATK;
				pWeapons[k]->Dulled();
				if (pWeapons[k]->ATK == 0){
					delete pWeapons[k];
					pWeapons[k] = NULL;
				}
			}
		}
		p->Hurted(ATK + wpATK);
		p->FightBack(this);
	}
	void FightBack(CWarrior *p){}
	int get_wpATK(){
		int wpATK = 0;
		if (pWeapons[0] != NULL&&pWeapons[0]->ID == SWORD)    wpATK = pWeapons[0]->ATK;
		else if (pWeapons[1] != NULL&&pWeapons[1]->ID == SWORD)	   wpATK = pWeapons[1]->ATK;
		return wpATK;
	}
	void turn_in_weapons(CWolf *p);
	void print_Weapon();
};
class CIceman :public CWarrior{
public:
	CWeapon *pWeapon;
	CIceman(CHeadquarter *p, int id) :CWarrior(p, id){
		warr_type = ICEMAN;
		HP = InitialHP[ICEMAN];
		ATK = InitialATK[ICEMAN];
		pWeapon = NULL;
		switch (ID % 3)
		{
		case SWORD:
			if (ATK*0.2 >= 1){
				pWeapon = new CSword(ATK*0.2);
			}
			break;
		case BOMB:pWeapon = new CBomb; break;
		case ARROW:pWeapon = new CArrow; break;
		default:
			break;
		}
	}
	~CIceman(){
		if (pWeapon)    delete pWeapon;
	}
	bool has_bomb(){
		if (pWeapon != NULL&&pWeapon->ID == BOMB)    return true;
		return false;
	}
	void print_IniAttr(){
		CWarrior::print_IniAttr();
	}
	void release_arrow();
	void Attack(CWarrior *p){
		if (isDead())    return;
		CWarrior::Attack(p);
		int wpATK = 0;
		if (pWeapon != NULL&&pWeapon->ID == SWORD){
			wpATK = pWeapon->ATK;
			pWeapon->Dulled();
			if (pWeapon->ATK == 0){
				delete pWeapon;
				pWeapon = NULL;
			}
		}
		p->Hurted(ATK + wpATK);
		p->FightBack(this);
	}
	void FightBack(CWarrior *p){
		if (isDead())    return;
		CWarrior::FightBack(p);
		int wpATK = 0;
		if (pWeapon != NULL&&pWeapon->ID == SWORD){
			wpATK = pWeapon->ATK;
			pWeapon->Dulled();
			if (pWeapon->ATK == 0){
				delete pWeapon;
				pWeapon = NULL;
			}
		}
		p->Hurted(ATK / 2 + wpATK);
	}
	int get_wpATK(){
		int wpATK = 0;
		if (pWeapon != NULL&&pWeapon->ID == SWORD)    wpATK = pWeapon->ATK;
		return wpATK;
	}
	void print_Weapon();
	void turn_in_weapons(CWolf *p);
};
class CLion :public CWarrior{
public:
	int loyalty;
	CLion(CHeadquarter *p, int id) :CWarrior(p, id){
		warr_type = LION;
		HP = InitialHP[LION];
		ATK = InitialATK[LION];
		loyalty = pHeadquarter->LifePoint;
	}
	void Attack(CWarrior *p){
		if (isDead())    return;
		CWarrior::Attack(p);
		p->Hurted(ATK);
		p->FightBack(this);
	}
	void FightBack(CWarrior *p){
		if (isDead())    return;
		CWarrior::FightBack(p);
		p->Hurted(ATK / 2);
	}
	void print_born(){}
	void print_IniAttr(){
		CWarrior::print_IniAttr();
		printf("Its loyalty is %d\n", loyalty);
	}
	bool lion_escaped(){
		if (loyalty <= 0)    return true;
		return false;
	}
	void print_City(){
		CWarrior::print_City();
	}
	void release_arrow(){}
	void change_loyalty(){
		if (!win())    loyalty -= K;
	}
	void print_Weapon();
};
class CWolf :public CWarrior{
public:
	CWeapon *pWeapons[3];//ָ��������ָ�룬�����������,0��1��2��Ӧ�����ı��
	CWolf(CHeadquarter *p, int id) :CWarrior(p, id){
		warr_type = WOLF;
		HP = InitialHP[WOLF];
		ATK = InitialATK[WOLF];
		memset(pWeapons, NULL, sizeof(pWeapons));
	}
	~CWolf(){
		for (int k = 0; k <= 2; ++k){
			if (pWeapons[k])    delete pWeapons[k];
		}
	}
	bool has_bomb(){
		if (pWeapons[BOMB] != NULL)    return true;
		return false;
	}
	void print_IniAttr(){
		CWarrior::print_IniAttr();
	}
	void print_City(){
		CWarrior::print_City();
	}
	void release_arrow();
	void Attack(CWarrior *p){
		if (isDead())    return;
		CWarrior::Attack(p);
		int wpATK = 0;
		if (pWeapons[SWORD] != NULL){
			wpATK = pWeapons[SWORD]->ATK;
			pWeapons[SWORD]->Dulled();
			if (pWeapons[SWORD]->ATK == 0){
				delete pWeapons[SWORD];
				pWeapons[SWORD] = NULL;
			}
		}
		p->Hurted(ATK + wpATK);
		p->FightBack(this);
	}
	void FightBack(CWarrior *p){
		if (isDead())    return;
		CWarrior::FightBack(p);
		int wpATK = 0;
		if (pWeapons[SWORD] != NULL){
			wpATK = pWeapons[SWORD]->ATK;
			pWeapons[SWORD]->Dulled();
			if (pWeapons[SWORD]->ATK == 0){
				delete pWeapons[SWORD];
				pWeapons[SWORD] = NULL;
			}
		}
		p->Hurted(ATK / 2 + wpATK);
	}
	void capture_weapons(CWarrior *p){
		if (!win())    return;
		p->turn_in_weapons(this);
	}
	int get_wpATK(){
		int wpATK = 0;
		if (pWeapons[SWORD] != NULL)    wpATK = pWeapons[SWORD]->ATK;
		return wpATK;
	}
	void turn_in_weapons(CWolf *p);
	void print_Weapon();
};

const string CWeapon::Names[3] = { "sword", "bomb", "arrow" };

void CCity::PrintMove(){
	for (int k = 0; k <= 1; ++k){
		if (pWarriors[k] != NULL){
			if (ID == 0 || ID == N + 1){
				if (pWarriors[k]->reach_headq == false){
					printf("%03d:%02d %s %s %d reached %s headquarter with %d elements and force %d\n",
						hour, minute, Color[pWarriors[k]->color].c_str(), CWarrior::Names[pWarriors[k]->warr_type].c_str(), pWarriors[k]->ID, Color[1 - pWarriors[k]->color].c_str(), pWarriors[k]->HP, pWarriors[k]->ATK);
					pWarriors[k]->reach_headq = true;
				}	
				//�ж�˾���ռ��
				//���������003:10 blue headquarter was taken
				if (ID == N + 1&&red_headquarter.Win){
					printf("%03d:%02d blue headquarter was taken\n", hour, minute);
					GameOver = true;
				}
				if (ID == 0&&blue_headquarter.Win){
					printf("%03d:%02d red headquarter was taken\n", hour, minute);
					GameOver = true;
				}
			}
			else    pWarriors[k]->print_City();
		}
	}
}
void CCity::Init(int id){
	ID = id;
	flag = -1;
	LifePoint = 0;
	memset(pWarriors, NULL, sizeof(pWarriors));
}
void CCity::WarrGetLP1(){
	if (pWarriors[RED] == NULL&&pWarriors[BLUE] != NULL){
		blue_headquarter.LifePoint += LifePoint;
		printf("%03d:%02d blue %s %d earned %d elements for his headquarter\n",
			hour, minute, CWarrior::Names[pWarriors[BLUE]->warr_type].c_str(), pWarriors[BLUE]->ID, LifePoint);
		LifePoint = 0;
	}
	else if (pWarriors[RED] != NULL&&pWarriors[BLUE] == NULL){
		red_headquarter.LifePoint += LifePoint;
		printf("%03d:%02d red %s %d earned %d elements for his headquarter\n",
			hour, minute, CWarrior::Names[pWarriors[RED]->warr_type].c_str(), pWarriors[RED]->ID, LifePoint);
		LifePoint = 0;
	}
}
void CCity::ReleaseArrow(){
	for (int k = 0; k <= 1; ++k){
		if (pWarriors[k] != NULL)    pWarriors[k]->release_arrow();
	}
}
void CCity::LionEscape(){
	if (pWarriors[RED] != NULL&&pWarriors[RED]->lion_escaped()&&ID != N + 1){
		printf("%03d:%02d red lion %d ran away\n", hour, minute, pWarriors[RED]->ID);
		//ָ�����ܵ�lion��ָ��ȫ����ΪNULL
		pWarriors[RED]->pHeadquarter->pWarriors[pWarriors[RED]->ID] = NULL;
		delete pWarriors[RED];
		pWarriors[RED] = NULL;
	}
	if (pWarriors[BLUE] != NULL&&pWarriors[BLUE]->lion_escaped() && ID != 0){
		printf("%03d:%02d blue lion %d ran away\n", hour, minute, pWarriors[BLUE]->ID);
		pWarriors[BLUE]->pHeadquarter->pWarriors[pWarriors[BLUE]->ID] = NULL;
		delete pWarriors[BLUE];
		pWarriors[BLUE] = NULL;
	}
}
void CCity::UseBomb(){
	if (pWarriors[RED] == NULL || pWarriors[BLUE] == NULL)    return;
	if (pWarriors[RED]->isDead() || pWarriors[BLUE]->isDead())    return;
	//��ʱ�洢������ʿ�����ԣ�ģ��ս�����̣��ж��Ƿ���Ҫʹ��ը��
	for (int k = RED; k <= BLUE; ++k){
		if (pWarriors[k] == NULL || (pWarriors[k] != NULL) && (pWarriors[k]->has_bomb() == false))    continue;
		int tmp_rHP = pWarriors[RED]->HP, tmp_bHP = pWarriors[BLUE]->HP;
		int tmp_rATK = pWarriors[RED]->ATK, tmp_bATK = pWarriors[BLUE]->ATK;
		int tmp_rwpATK = pWarriors[RED]->get_wpATK(), tmp_bwpATK = pWarriors[BLUE]->get_wpATK();
		bool usebomb = false;
		if (k == RED){
			if (flag == RED || (flag == -1 && ID % 2 == 1)){
				tmp_bHP = tmp_bHP - tmp_rATK - tmp_rwpATK;
				if (tmp_bHP > 0&&pWarriors[BLUE]->warr_type != NINJA){
					tmp_rHP = tmp_rHP - (tmp_bATK / 2) - tmp_bwpATK;
					if (tmp_rHP <= 0)     usebomb = true;
				}
			}
			else if (flag == BLUE || (flag == -1 && ID % 2 == 0)){
				tmp_rHP = tmp_rHP - tmp_bATK - tmp_bwpATK;
				if (tmp_rHP <= 0)     usebomb = true;
			}
			if (usebomb){
				printf("%03d:%02d red %s %d used a bomb and killed blue %s %d\n",
					hour, minute, CWarrior::Names[pWarriors[RED]->warr_type].c_str(), pWarriors[RED]->ID, CWarrior::Names[pWarriors[BLUE]->warr_type].c_str(), pWarriors[BLUE]->ID);
				red_headquarter.pWarriors[pWarriors[RED]->ID] = NULL;
				blue_headquarter.pWarriors[pWarriors[BLUE]->ID] = NULL;
				for (int l = RED; l <= BLUE; ++l){
					delete pWarriors[l];
					pWarriors[l] = NULL;
				}
			}
		}
		else if (k == BLUE){
			if (flag == BLUE || (flag == -1 && ID % 2 == 0)){
				tmp_rHP = tmp_rHP - tmp_bATK - tmp_bwpATK;
				if (tmp_rHP > 0&&pWarriors[RED]->warr_type != NINJA){
					tmp_bHP = tmp_bHP - (tmp_rATK / 2) - tmp_rwpATK;
					if (tmp_bHP <= 0)     usebomb = true;
				}
			}
			else if (flag == RED || (flag == -1 && ID % 2 == 1)){
				tmp_bHP = tmp_bHP - tmp_rATK - tmp_rwpATK;
				if (tmp_bHP <= 0)     usebomb = true;
			}
			if (usebomb){
				printf("%03d:%02d blue %s %d used a bomb and killed red %s %d\n",
					hour, minute, CWarrior::Names[pWarriors[BLUE]->warr_type].c_str(), pWarriors[BLUE]->ID, CWarrior::Names[pWarriors[RED]->warr_type].c_str(), pWarriors[RED]->ID);
				red_headquarter.pWarriors[pWarriors[RED]->ID] = NULL;
				blue_headquarter.pWarriors[pWarriors[BLUE]->ID] = NULL;
				for (int l = RED; l <= BLUE; ++l){
					delete pWarriors[l];
					pWarriors[l] = NULL;
				}
			}
		}
	}
}
void CCity::Battle(){
	if (pWarriors[RED] == NULL || pWarriors[BLUE] == NULL)    return;
	if (pWarriors[RED]->isDead() && pWarriors[BLUE]->isDead())    return;
	//���������5����ǰ�Ѿ���������arrow��������Ȼ��Ϊ������һ��ս���������ʤ��
	//������²����С���ʿ����������������ʿ������������ʿս�������¼���������ս��ʤ����Ӧ�÷��������鶼�ᷢ��
	//��������
	bool b = false;
	for (int k = RED; k <= BLUE; ++k){
		if (pWarriors[k]->isDead() == true && (pWarriors[1 - k]->isDead() == false)){
			b = true;
			if (pWarriors[1 - k]->warr_type == DRAGON){
				pWarriors[1 - k]->change_morale();
				if (pWarriors[1 - k]->yell() && (flag == 1 - k || (flag == -1 && ID % 2 == k))){
					printf("%03d:%02d %s dragon %d yelled in city %d\n",
						hour, minute, Color[1 - k].c_str(), pWarriors[1 - k]->ID, ID);
				}
			}
			if (pWarriors[1 - k]->warr_type == WOLF)    pWarriors[1 - k]->capture_weapons(pWarriors[k]);
		}
		if (k == 1&&b){
			WarrGetLP2();
			RecordVictory();
			ChangeFlag();
			return;
		}
	}
	//�����ĳ��У��Լ����Ϊ������������У��ɺ���ʿ�������������
	//������ĳ��У��Լ����Ϊż����������У�������ʿ�����������
	int tmp_lionHP[2] = { pWarriors[RED]->HP, pWarriors[BLUE]->HP };
	if (flag == RED || (flag == -1 && ID % 2 == 1)){
		pWarriors[RED]->Attack(pWarriors[BLUE]);//�Ѿ�����fightback

	}
	else if (flag == BLUE || (flag == -1 && ID % 2 == 0)){
		pWarriors[BLUE]->Attack(pWarriors[RED]);
	}
	Killed();
	for (int k = RED; k <= BLUE; ++k){
		if (pWarriors[k]->warr_type == DRAGON){
			pWarriors[k]->change_morale();
			if (pWarriors[k]->yell() && (flag == k || (flag == -1 && ID % 2 == 1 - k))){
				printf("%03d:%02d %s dragon %d yelled in city %d\n",
					hour, minute, Color[k].c_str(), pWarriors[k]->ID, ID);
			}
		}
		if (pWarriors[k]->warr_type == LION){
			if (!pWarriors[k]->win())    pWarriors[k]->change_loyalty();
			if (pWarriors[k]->isDead())    pWarriors[1 - k]->HP += tmp_lionHP[k];
		}
		if (pWarriors[k]->warr_type == WOLF&&pWarriors[k]->win())    pWarriors[k]->capture_weapons(pWarriors[1 - k]);
	}
	WarrGetLP2();
	RecordVictory();
	ChangeFlag();
}
void CCity::Killed(){
	if (pWarriors[RED] == NULL || pWarriors[BLUE] == NULL)    return;
	//���������001:40 red lion 2 was killed in city 1
	if (pWarriors[RED]->isDead())    printf("%03d:%02d red %s %d was killed in city %d\n",
		hour, minute, CWarrior::Names[pWarriors[RED]->warr_type].c_str(), pWarriors[RED]->ID, ID);
	if (pWarriors[BLUE]->isDead())    printf("%03d:%02d blue %s %d was killed in city %d\n",
		hour, minute, CWarrior::Names[pWarriors[BLUE]->warr_type].c_str(), pWarriors[BLUE]->ID, ID);
}
void CCity::DisposeBody(){
	for (int k = RED; k <= BLUE; ++k){
		if (pWarriors[k] != NULL&&pWarriors[k]->isDead()){
			pWarriors[k]->pHeadquarter->pWarriors[pWarriors[k]->ID] = NULL;
			delete pWarriors[k];
			pWarriors[k] = NULL;
		}
	}
}
void CCity::WarrGetLP2(){
	//���������001:40 blue dragon 2 earned 10 elements for his headquarter
	if (pWarriors[RED] == NULL || pWarriors[BLUE] == NULL)    return;
	if (pWarriors[RED]->win()){
		printf("%03d:%02d red %s %d earned %d elements for his headquarter\n",
			hour, minute, CWarrior::Names[pWarriors[RED]->warr_type].c_str(), pWarriors[RED]->ID, LifePoint);
		pWarriors[RED]->pHeadquarter->LifePoint += LifePoint;
		LifePoint = 0;
	}
	else if (pWarriors[BLUE]->win()){
		printf("%03d:%02d blue %s %d earned %d elements for his headquarter\n",
			hour, minute, CWarrior::Names[pWarriors[BLUE]->warr_type].c_str(), pWarriors[BLUE]->ID, LifePoint);
		pWarriors[BLUE]->pHeadquarter->LifePoint += LifePoint;
		LifePoint = 0;
	}
}
void CCity::RecordVictory(){
	if (pWarriors[RED] == NULL || pWarriors[BLUE] == NULL)    return;
	battle_num += 1;//record[0]Ϊ-1����ƽ��
	if (pWarriors[RED]->win())    record[battle_num] = RED;
	else if (pWarriors[BLUE]->win())    record[battle_num] = BLUE;
}
void CCity::ChangeFlag(){
	//��ĳ����������������ս������ͬһ������ʿɱ������
	if (battle_num >= 2){
		//���������004:40 blue flag raised in city 4
		if (record[battle_num] == RED&&record[battle_num - 1] == RED&&record[battle_num - 2] != RED){
			if (flag != RED){
				flag = RED;
				printf("%03d:%02d red flag raised in city %d\n", hour, minute, ID);
			}
		}
		else if (record[battle_num] == BLUE&&record[battle_num - 1] == BLUE&&record[battle_num - 2] != BLUE){
			if (flag != BLUE){
				flag = BLUE;
				printf("%03d:%02d blue flag raised in city %d\n", hour, minute, ID);
			}
		}
	}
}
void CCity::redWarrReportWP(){
	if (pWarriors[RED] == NULL)    return;
	pWarriors[RED]->print_Weapon();
}
void CCity::blueWarrReportWP(){
	if (pWarriors[BLUE] == NULL)    return;
	pWarriors[BLUE]->print_Weapon();
}

int CHeadquarter::InitialLifePoint = 0;
int CHeadquarter::MakingSeq[2][5] = { { 2, 3, 4, 1, 0 }, { 3, 0, 1, 2, 4 } };
void CHeadquarter::Init(int c, int M){
	color = c;
	LifePoint = M;
	warr_num = 0;
	memset(pWarriors, NULL, sizeof(pWarriors));
	Win = false;
}
void CHeadquarter::ProduceWarr(){
	//�췽˾�����iceman��lion��wolf��ninja��dragon��˳��ѭ��������ʿ
	//����˾�����lion��dragon��ninja��iceman��wolf��˳��ѭ��������ʿ�� 
	int this_warr = MakingSeq[color][warr_num % 5];
	if (LifePoint < CWarrior::InitialHP[this_warr])    return;
	warr_num += 1;
	LifePoint -= CWarrior::InitialHP[this_warr];
	//��ʿ�ı�ž���˾�����ʿ������±�
	switch (this_warr)
	{
	//��ħ����˾��������Ϊ0�ĳ��У���ħ����˾��������ΪN+1�ĳ���
	case DRAGON:pWarriors[warr_num] = new CDragon(this, warr_num); break;
	case NINJA:pWarriors[warr_num] = new CNinja(this, warr_num); break;
	case ICEMAN:pWarriors[warr_num] = new CIceman(this, warr_num); break;
	case LION:pWarriors[warr_num] = new CLion(this, warr_num); break;
	case WOLF:pWarriors[warr_num] = new CWolf(this, warr_num); break;
	default:
		break;
	}
	pWarriors[warr_num]->print_IniAttr();
}
void CHeadquarter::WarrMove(){
	if (warr_num == 0)    return;
	for (int i = 1; i <= warr_num; ++i){
		if (pWarriors[i])    pWarriors[i]->move();
	}
}
void CHeadquarter::Reward(){
	if (warr_num == 0)     return;
	for (int k = warr_num; k >= 1; --k){
		if (pWarriors[k] != NULL){
			if (pWarriors[k]->win()){
				if (tmpLifePoint < 8)    break;
				tmpLifePoint -= 8;
				LifePoint -= 8;
				pWarriors[k]->HP += 8;
			}
		}
	}
}
void CHeadquarter::ReportLP(){
	//���������000:50 100 elements in red headquarter 
	//          000:50 120 elements in blue headquarter
	printf("%03d:%02d %d elements in %s headquarter\n", hour, minute, LifePoint, Color[color].c_str());
}

int CWarrior::InitialHP[5] = {};
int CWarrior::InitialATK[5] = {};
const string CWarrior::Names[5] = { "dragon", "ninja", "iceman", "lion", "wolf" };
void CWarrior::print_IniAttr(){
	printf("%03d:%02d %s %s %d born\n", hour, minute, Color[color].c_str(), Names[warr_type].c_str(), ID);
}
void CWarrior::print_City(){
	if (pC_ID <= N&&pC_ID >= 1)    printf("%03d:%02d %s %s %d marched to city %d with %d elements and force %d\n",
		hour, minute, Color[color].c_str(), Names[warr_type].c_str(), ID, pC_ID, HP, ATK);
}
void CWarrior::move(){
	if (color == RED){
		if (pC_ID == N + 1)    return;
		pC_ID += 1;
		steps += 1; 
		if (steps % 2 == 0 && warr_type == ICEMAN){
			HP -= 9;
			ATK += 20;
			if (HP <= 0)    HP = 1;
		}
		pCity->pWarriors[RED] = NULL;
		pCity = NULL;
		pCity = &pCities[pC_ID];
		if (pC_ID == N + 1 && pCity->pWarriors[RED] != NULL){
			pHeadquarter->Win = true;
		}
		pCity->pWarriors[RED] = this;
	}
	else if (color == BLUE){
		if (pC_ID == 0)    return;
		pC_ID -= 1;
		steps += 1;
		if (steps % 2 == 0 && warr_type == ICEMAN){
			HP -= 9;
			ATK += 20;
			if (HP <= 0)    HP = 1;
		}
		pCity->pWarriors[BLUE] = NULL;
		pCity = NULL;
		pCity = &pCities[pC_ID];
		if (pC_ID == 0 && pCity->pWarriors[BLUE] != NULL){
			pHeadquarter->Win = true;
		}
		pCity->pWarriors[BLUE] = this;
	}
}
void CDragon::release_arrow(){
	//�����˾���˾�֮ǰ���Ǹ����У����ܷż�
	if ((color == RED&&pC_ID >= N) || (color == BLUE&&pC_ID <= 1))    return;
	//����м�
	if (pWeapon != NULL && (pWeapon->ID == ARROW) && (pWeapon->count_used < 3)){
		if (color == RED){
			if (pCities[pC_ID + 1].pWarriors[BLUE] != NULL){
				pCities[pC_ID + 1].pWarriors[BLUE]->HP -= R;
				printf("%03d:%02d red %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
				if (pCities[pC_ID + 1].pWarriors[BLUE]->isDead()){
					printf(" and killed blue %s %d",
						Names[pCities[pC_ID + 1].pWarriors[BLUE]->warr_type].c_str(), pCities[pC_ID + 1].pWarriors[BLUE]->ID);
				}
				printf("\n");
				pWeapon->count_used += 1;
				if (pWeapon->count_used == 3){
					delete pWeapon;
					pWeapon = NULL;//ʧЧ������ֱ��delete
				}
			}
		}
		else if (color = BLUE){
			if (pCities[pC_ID - 1].pWarriors[RED] != NULL){
				pCities[pC_ID - 1].pWarriors[RED]->HP -= R;
				printf("%03d:%02d blue %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
				if (pCities[pC_ID - 1].pWarriors[RED]->isDead()){
					printf(" and killed red %s %d",
						Names[pCities[pC_ID - 1].pWarriors[RED]->warr_type].c_str(), pCities[pC_ID - 1].pWarriors[RED]->ID);
				}
				printf("\n");
				pWeapon->count_used += 1;
				if (pWeapon->count_used == 3){
					delete pWeapon;
					pWeapon = NULL;//ʧЧ������ֱ��delete
				}
			}
		}
	}
}
void CNinja::release_arrow(){
	//�����˾���˾�֮ǰ���Ǹ����У����ܷż�
	if ((color == RED&&pC_ID >= N) || (color == BLUE&&pC_ID <= 1))    return;
	//����м�
	for (int k = 0; k <= 1; ++k){//һ��ֻ��һ��������arrow
		if (pWeapons[k] != NULL && (pWeapons[k]->ID == ARROW) && (pWeapons[k]->count_used < 3)){
			if (color == RED){
				if (pCities[pC_ID + 1].pWarriors[BLUE] != NULL){
					pCities[pC_ID + 1].pWarriors[BLUE]->HP -= R;
					printf("%03d:%02d red %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
					if (pCities[pC_ID + 1].pWarriors[BLUE]->isDead()){
						printf(" and killed blue %s %d",
							Names[pCities[pC_ID + 1].pWarriors[BLUE]->warr_type].c_str(), pCities[pC_ID + 1].pWarriors[BLUE]->ID);
					}
					printf("\n");
					pWeapons[k]->count_used += 1;
					if (pWeapons[k]->count_used == 3){
						delete pWeapons[k];
						pWeapons[k] = NULL;//ʧЧ������ֱ��delete
					}
				}
			}
			else if (color = BLUE){
				if (pCities[pC_ID - 1].pWarriors[RED] != NULL){
					pCities[pC_ID - 1].pWarriors[RED]->HP -= R;
					printf("%03d:%02d blue %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
					if (pCities[pC_ID - 1].pWarriors[RED]->isDead()){
						printf(" and killed red %s %d",
							Names[pCities[pC_ID - 1].pWarriors[RED]->warr_type].c_str(), pCities[pC_ID - 1].pWarriors[RED]->ID);
					}
					printf("\n");
					pWeapons[k]->count_used += 1;
					if (pWeapons[k]->count_used == 3){
						delete pWeapons[k];
						pWeapons[k] = NULL;//ʧЧ������ֱ��delete
					}
				}
			}
		}
	}
}
void CIceman::release_arrow(){
	//�����˾���˾�֮ǰ���Ǹ����У����ܷż�
	if ((color == RED&&pC_ID >= N) || (color == BLUE&&pC_ID <= 1))    return;
	//����м�
	if (pWeapon != NULL && (pWeapon->ID == ARROW) && (pWeapon->count_used < 3)){
		if (color == RED){
			if (pCities[pC_ID + 1].pWarriors[BLUE] != NULL){
				pCities[pC_ID + 1].pWarriors[BLUE]->HP -= R;
				printf("%03d:%02d red %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
				if (pCities[pC_ID + 1].pWarriors[BLUE]->isDead()){
					printf(" and killed blue %s %d",
						Names[pCities[pC_ID + 1].pWarriors[BLUE]->warr_type].c_str(), pCities[pC_ID + 1].pWarriors[BLUE]->ID);
				}
				printf("\n");
				pWeapon->count_used += 1;
				if (pWeapon->count_used == 3){
					delete pWeapon;
					pWeapon = NULL;//ʧЧ������ֱ��delete
				}
			}
		}
		else if (color = BLUE){
			if (pCities[pC_ID - 1].pWarriors[RED] != NULL){
				pCities[pC_ID - 1].pWarriors[RED]->HP -= R;
				printf("%03d:%02d blue %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
				if (pCities[pC_ID - 1].pWarriors[RED]->isDead()){
					printf(" and killed red %s %d",
						Names[pCities[pC_ID - 1].pWarriors[RED]->warr_type].c_str(), pCities[pC_ID - 1].pWarriors[RED]->ID);
				}
				printf("\n");
				pWeapon->count_used += 1;
				if (pWeapon->count_used == 3){
					delete pWeapon;
					pWeapon = NULL;//ʧЧ������ֱ��delete
				}
			}
		}
	}
}
void CWolf::release_arrow(){
	//�����˾���˾�֮ǰ���Ǹ����У����ܷż�
	if ((color == RED&&pC_ID >= N) || (color == BLUE&&pC_ID <= 1))    return;
	//����м�
	if (pWeapons[ARROW] != NULL && (pWeapons[ARROW]->count_used < 3)){
		if (color == RED){
			if (pCities[pC_ID + 1].pWarriors[BLUE] != NULL){
				pCities[pC_ID + 1].pWarriors[BLUE]->HP -= R;
				printf("%03d:%02d red %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
				if (pCities[pC_ID + 1].pWarriors[BLUE]->isDead()){
					printf(" and killed blue %s %d",
						Names[pCities[pC_ID + 1].pWarriors[BLUE]->warr_type].c_str(), pCities[pC_ID + 1].pWarriors[BLUE]->ID);
				}
				printf("\n");
				pWeapons[ARROW]->count_used += 1;
				if (pWeapons[ARROW]->count_used == 3){
					delete pWeapons[ARROW];
					pWeapons[ARROW] = NULL;
				}
			}
		}
		else if (color == BLUE){
			if (pCities[pC_ID - 1].pWarriors[RED] != NULL){
				pCities[pC_ID - 1].pWarriors[RED]->HP -= R;
				printf("%03d:%02d blue %s %d shot", hour, minute, Names[warr_type].c_str(), ID);
				if (pCities[pC_ID - 1].pWarriors[RED]->isDead()){
					printf(" and killed red %s %d",
						Names[pCities[pC_ID - 1].pWarriors[RED]->warr_type].c_str(), pCities[pC_ID - 1].pWarriors[RED]->ID);
				}
				printf("\n");
				pWeapons[ARROW]->count_used += 1;
				if (pWeapons[ARROW]->count_used == 3){
					delete pWeapons[ARROW];
					pWeapons[ARROW] = NULL;
				}
			}
		}
	}
}
void CDragon::turn_in_weapons(CWolf *p){
	if (pWeapon != NULL){
		switch (pWeapon->ID)
		{
		case SWORD:
			if (p->pWeapons[SWORD] == NULL){
				p->pWeapons[SWORD] = pWeapon;
				pWeapon = NULL;
			}
			break;
		case BOMB:
			if (p->pWeapons[BOMB] == NULL){
				p->pWeapons[BOMB] = pWeapon;
				pWeapon = NULL;
			}
			break;
		case ARROW:
			if (p->pWeapons[ARROW] == NULL){
				p->pWeapons[ARROW] = pWeapon;
				pWeapon = NULL;
			}
			break;
		default:
			break;
		}
	}
}
void CNinja::turn_in_weapons(CWolf *p){
	for (int k = 0; k <= 1; ++k){
		if (pWeapons[k] != NULL){
			switch (pWeapons[k]->ID)
			{
			case SWORD:
				if (p->pWeapons[SWORD] == NULL){
					p->pWeapons[SWORD] = pWeapons[k];
					pWeapons[k] = NULL;
				}
				break;
			case BOMB:
				if (p->pWeapons[BOMB] == NULL){
					p->pWeapons[BOMB] = pWeapons[k];
					pWeapons[k] = NULL;
				}
				break;
			case ARROW:
				if (p->pWeapons[ARROW] == NULL){
					p->pWeapons[ARROW] = pWeapons[k];
					pWeapons[k] = NULL;
				}
				break;
			default:
				break;
			}
		}
	}
}
void CIceman::turn_in_weapons(CWolf *p){
	if (pWeapon != NULL){
		switch (pWeapon->ID)
		{
		case SWORD:
			if (p->pWeapons[SWORD] == NULL){
				p->pWeapons[SWORD] = pWeapon;
				pWeapon = NULL;
			}
			break;
		case BOMB:
			if (p->pWeapons[BOMB] == NULL){
				p->pWeapons[BOMB] = pWeapon;
				pWeapon = NULL;
			}
			break;
		case ARROW:
			if (p->pWeapons[ARROW] == NULL){
				p->pWeapons[ARROW] = pWeapon;
				pWeapon = NULL;
			}
			break;
		default:
			break;
		}
	}
}
void CWolf::turn_in_weapons(CWolf *p){
	for (int k = 0; k <= 2; ++k){
		if (pWeapons[k] != NULL&&p->pWeapons[k] == NULL){
			p->pWeapons[k] = pWeapons[k];
			pWeapons[k] = NULL;
		}
	}
}
//���˳��arrow,bomb,sword   arrow(ʣ��ʹ�ô���)��sword(������)
//000:55 blue wolf 2 has arrow(2), bomb, sword(23)
//000:55 blue wolf 4 has no weapon
//000:55 blue wolf 5 has sword(20)
void CDragon::print_Weapon(){
	printf("%03d:%02d %s dragon %d has ", hour, minute, Color[color].c_str(), ID);
	if (pWeapon == NULL)    printf("no weapon");
	else if (pWeapon->ID == ARROW)    printf("arrow(%d)", 3 - pWeapon->count_used);
	else if (pWeapon->ID == BOMB)    printf("bomb");
	else if (pWeapon->ID == SWORD)    printf("sword(%d)", pWeapon->ATK);
	printf("\n");
}
void CNinja::print_Weapon(){
	printf("%03d:%02d %s ninja %d has ", hour, minute, Color[color].c_str(), ID);
	bool has_arrow = false, has_bomb = false, has_sword = false;
	int arr_used = 0, swdATK = 0;
	if (pWeapons[0] ==NULL && pWeapons[1] == NULL)    printf("no weapon");
	else{
		for (int k = 0; k <= 1; ++k){
			if (pWeapons[k] != NULL&&pWeapons[k]->ID == ARROW){
				has_arrow = true;
				arr_used = pWeapons[k]->count_used;
			}
			else if (pWeapons[k] != NULL&&pWeapons[k]->ID == BOMB)    has_bomb = true;
			else if (pWeapons[k] != NULL&&pWeapons[k]->ID == SWORD){
				has_sword = true;
				swdATK = pWeapons[k]->ATK;
			}
		}
		if (has_arrow)    printf("arrow(%d)", 3 - arr_used);
		if (has_bomb){
			if (has_arrow)    printf(",");
			printf("bomb");
		}
		if (has_sword){
			if (has_arrow || has_bomb)     printf(",");
			printf("sword(%d)", swdATK);
		}
	}
	printf("\n");
}
void CIceman::print_Weapon(){
	printf("%03d:%02d %s iceman %d has ", hour, minute, Color[color].c_str(), ID);
	if (pWeapon == NULL)    printf("no weapon");
	else if (pWeapon->ID == ARROW)    printf("arrow(%d)", 3 - pWeapon->count_used);
	else if (pWeapon->ID == BOMB)    printf("bomb");
	else if (pWeapon->ID == SWORD)    printf("sword(%d)", pWeapon->ATK);
	printf("\n");
}
void CLion::print_Weapon(){
	printf("%03d:%02d %s lion %d has no weapon\n", hour, minute, Color[color].c_str(), ID);
}
void CWolf::print_Weapon(){
	printf("%03d:%02d %s wolf %d has ", hour, minute, Color[color].c_str(), ID);
	if (pWeapons[ARROW] == NULL&&pWeapons[BOMB] == NULL&&pWeapons[SWORD] == NULL)    
		printf("no weapon");
	else{
		if (pWeapons[ARROW] != NULL)     printf("arrow(%d)", 3 - pWeapons[ARROW]->count_used);
		if (pWeapons[BOMB] != NULL){
			if (pWeapons[ARROW] != NULL)    printf(",");
			printf("bomb");
		}
		if (pWeapons[SWORD] != NULL){
			if (pWeapons[ARROW] != NULL || pWeapons[BOMB] != NULL)
				printf(",");
			printf("sword(%d)", pWeapons[SWORD]->ATK);
		}
	}
	printf("\n");
}

int main(){
	int n;//��������
	cin >> n;
	for (int i = 1; i <= n; ++i){

		//���ó�ʼ����
		cin >> M >> N >> R >> K >> T;
		for (int j = 0; j < 5; ++j)    cin >> CWarrior::InitialHP[j];
		for (int j = 0; j < 5; ++j)    cin >> CWarrior::InitialATK[j];
		red_headquarter.Init(RED, M);
		blue_headquarter.Init(BLUE, M);
		pCities = new CCity[N + 2];
		for (int j = 0; j <= N + 1; ++j)    pCities[j].Init(j);

		printf("Case %d:\n", i);

		int totalTime = T / 60 * 60;
		for (hour = 0; hour <= T / 60; ++hour){

			//1��00 ��ʿ����
			minute = 0;
			if (hour == T / 60 && totalTime > T)    break;
			red_headquarter.ProduceWarr();
			blue_headquarter.ProduceWarr();

			//2��05 lion����
			minute = 5;
			if (hour == T / 60)    totalTime += 5;
			if (hour == T / 60 && totalTime > T)    break;
			for (int j = 0; j <= N + 1; ++j)    pCities[j].LionEscape();

			//3��10 ��ʿǰ����ĳһ����
			minute = 10;
			if (hour == T / 60)    totalTime += 5;
			if (hour == T / 60 && totalTime > T)    break;
			red_headquarter.WarrMove();
			blue_headquarter.WarrMove();
			for (int j = 0; j <= N + 1; ++j)    pCities[j].PrintMove();
			if (GameOver)    break;

			//4��20 ÿ�����в���10������Ԫ
			minute = 20;
			if (hour == T / 60)    totalTime += 10;
			if (hour == T / 60 && totalTime > T)    break;
			for (int j = 1; j <= N; ++j)   pCities[j].LifePoint += 10;

			//5��30 ���ĳ��������ֻ��һ����ʿ����ô����ʿȡ�߸ó����е���������Ԫ������������Щ����Ԫ���͵���������˾���
			minute = 30;
			if (hour == T / 60)    totalTime += 10;
			if (hour == T / 60 && totalTime > T)    break;
			for (int j = 1; j <= N; ++j)    pCities[j].WarrGetLP1();

			//6��35 ��ʿ�ż�
			minute = 35;
			if (hour == T / 60)    totalTime += 5;
			if (hour == T / 60 && totalTime > T)    break;
			for (int j = 0; j <= N + 1; ++j)    pCities[j].ReleaseArrow();

			//7��38 ��ʿʹ��ը��
			minute = 38;
			if (hour == T / 60)    totalTime += 3;
			if (hour == T / 60 && totalTime > T)    break;
			for (int j = 1; j <= N; ++j)     pCities[j].UseBomb();

			//8��40 ս������ʿ������������ʿ��������ʿս��
			minute = 40;
			if (hour == T / 60)    totalTime += 2;
			if (hour == T / 60 && totalTime > T)    break;
			red_headquarter.tmpLifePoint = red_headquarter.LifePoint;
			blue_headquarter.tmpLifePoint = blue_headquarter.LifePoint;
			for (int j = 1; j <= N; ++j)    pCities[j].Battle();
			//˾�����սʤ����ʿ
			red_headquarter.Reward();
			blue_headquarter.Reward();
			//ɾ��ʬ��
			for (int j = 1; j <= N; ++j)    pCities[j].DisposeBody();

			//9��50 ˾���������Ԫ����
			minute = 50;
			if (hour == T / 60)    totalTime += 10;
			if (hour == T / 60 && totalTime > T)    break;
			red_headquarter.ReportLP();
			blue_headquarter.ReportLP();

			//10��55 ��ʿ�����������
			//�Ȱ������򶫵�˳�����еĺ���ʿ���棬Ȼ���ٴ��������е�����ʿ���档
			minute = 55;
			if (hour == T / 60)    totalTime += 5;
			if (hour == T / 60 && totalTime > T)    break;
			for (int j = 0; j <= N + 1; ++j)    pCities[j].redWarrReportWP();
			for (int j = 0; j <= N + 1; ++j)    pCities[j].blueWarrReportWP();
		}

		//����
		memset(CWarrior::InitialHP, 0, sizeof(CWarrior::InitialHP));
		memset(CWarrior::InitialATK, 0, sizeof(CWarrior::InitialATK));
		M = 0, N = 0, R = 0, K = 0, T = 0;
		delete[] pCities;
		for (int j = 0; j < 1000; ++j){
			if (red_headquarter.pWarriors[j])    delete red_headquarter.pWarriors[j];
			if (blue_headquarter.pWarriors[j])    delete blue_headquarter.pWarriors[j];
		}
		GameOver = false;
	}
	return 0;
}