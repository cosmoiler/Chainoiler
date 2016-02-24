#ifndef Scottoiler_h
#define Scottoiler_h

#include <inttypes.h>

#define K_OnSec 10 // ���
#define K_AllWheel -5 // � ��������
#define K_TEMP10_SEC +10
#define K_TEMP10_WHEEL -5
#define K_TEMP15_SEC +8
#define K_TEMP15_WHEEL -2
#define K_TEMP20_SEC +5
#define K_TEMP20_WHEEL -1
#define K_TEMP25_SEC -5
#define K_TEMP25_WHEEL +5
#define K_TEMP30_SEC -10
#define K_TEMP30_WHEEL +10

#ifdef __cplusplus
extern "C" {
#endif

class Scottoiler
{

private:
	Scottoiler();//uint8_t pin, uint8_t userOnSec, uint8_t userOnWheel, uint8_t userAllWheel
	static Scottoiler* _instance;
	uint8_t _nOn;
	uint16_t _nOff;

public:
    // Initialize

	static void init() { if (_instance == 0) _instance = new Scottoiler(); }
//	Scottoiler();
  static Scottoiler* instance() { return _instance; }

  void Setup(uint8_t pin, uint8_t userOnSec, uint8_t userOnWheel, uint16_t userOffWheel);
     // ���. ������
  void On();
    // ����. ������
  void Off();
    // ����� ���. ��������� �������
  uint16_t GetOnSec();
    // ����� �������� ������ ����� ������ �������
  uint8_t getOnWheel();

  uint8_t getNOnWheel();
	// ����� ����� ����� ������ ������� (�������� n)
  uint16_t GetOn_N();
    // ����� �������� ������ ����� ������ ��������
  uint16_t getOffWheel();

  uint16_t getNOffWheel();
	// ����� ����� ����� ������ �������� (�������� m)
  uint16_t GetOff_M();
	// ����� ������� (�������� = 0)
  uint8_t GetPumpPeriod();
    // ������ ������ �������
  uint16_t GetAllWheel();
    // ��������� ������� (���./����.)
  bool GetState();
  uint8_t GetMode();
  uint8_t GetBlobs();
  long int GetNon();
  int GetTemperature();
  uint8_t GetChainImp();

  void SetOnSec(uint16_t userOnSec);
  void SetOnWheel(uint8_t userOnWheel);
  void SetOffWheel(uint16_t userOffWheel);
  void SetAllWheel(uint16_t userAllWheel);
  void SetMode(uint8_t userMode);
  void SetOn_N(uint16_t userN);
  void SetOff_M(uint16_t userM);
  void SetPumpPeriod(uint8_t userK);
  void SetBlobs(uint8_t userBlobs);
  void Update(bool userOnPump);
  void SetTemperature(int userCelsius);
  void SetChainImp(uint8_t userChainImp);

protected:
  uint16_t _OnSec;       // ����� (� mS) ������ �����
  uint8_t _OnWheel;     // ���. ������ ������������ ����� ������� ������ ()
  //uint16_t _AllWheel;    // ������ ���. � ����. ������� (����� ������� ������)
  uint16_t _OffWheel;
  uint16_t _On_N;		// ���������� ����� ������ ���. (� �)
  uint16_t _Off_M;		// ���������� ����� ������ ����.(� ��)
  uint8_t _Sec_Period;		// ����� ������������ ����� (� ���, ��� ������ TIME)
  uint8_t _ChainImp;   // ����� ��������� �� 1 ������ ����
  bool _fStateValve; // ��������� �������
  uint8_t _mode;
  uint8_t _blobs;       // ����� ������ � ������
  long int _Non;		// ����� ��������� ������� ����� ���. ���������
  int _celsius;

  uint8_t pin;
};

#ifdef __cplusplus
}
#endif


#endif

