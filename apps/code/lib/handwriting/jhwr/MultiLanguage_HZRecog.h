#ifdef __cplusplus
extern "C"    {
#endif

#ifdef HZRECOG_EXPORTS
	#define	HZRECOGAPI __declspec( dllexport )
#else
	#define	HZRECOGAPI /*__declspec( dllimport )*/
#endif

#define HZ_VOID void
#define HZ_INT8 char
#define HZ_UINT8 unsigned char
#define HZ_INT16 short
#define HZ_UINT16 unsigned short
#define HZ_INT32 long				
#define HZ_UINT32 unsigned long		

#define HZ_BOOL HZ_INT32
#define HZ_BYTE HZ_UINT8			
#define HZ_WORD HZ_UINT16			
#define HZ_DWORD HZ_UINT32			

// Recognition Kernel Initialization
// call this function before using other functions in handwritting recognition kernel
// the pointer is ROM address for placing HZRecog.dat or is file name string
// Return value :	TRUE	success
//					FALSE	fail
HZRECOGAPI HZ_BOOL  HZInitCharacterRecognition_Multi(HZ_VOID* pPointer);

// Recognition Kernel Exit
// call this function before using other functions handwritting recognition kernel no more
// Return value :	TRUE	success
//					FALSE	fail
HZRECOGAPI HZ_BOOL  HZExitCharacterRecognition_Multi(HZ_VOID);

// Main Recognition Function
// Return value is recognition result number, its valid value are [-1, MAX_CANDIDATE_NUM]
// pnStrokeBuffer is handwritting buffer of Chinese character.
// (x, y) make up one point, and x and y are short type, their valid value are from -1 to 32767.
// (-1, 0) is end flag of stroke, (-1, -1) is end flag of character.
// pwResultBuffer is recognition result buffer, we recommend its size are MAX_CANDIDATE_NUM+1
// uMatchRange is one of four kinds of match_range or combination of them. Note: it is not null(zero)
// moreover, uMatchRange may include ADAPTATION_MATCH_RANGE only after calling 'HZInitAdaptation'
HZRECOGAPI HZ_INT32  HZCharacterRecognize_Multi(HZ_INT16* pnStrokeBuffer, HZ_WORD* pwResult);

#define RECOG_RANGE_NUMBER			0x00000001		// ���֣�0~9ʮ������
#define RECOG_RANGE_UPPERCASE		0x00000002		// ��д��ĸ��������Ӣ������26��
#define RECOG_RANGE_LOWERCASE		0x00000004		// Сд��ĸ��������Ӣ������26��
#define RECOG_RANGE_INTERPUNCTION	0x00000008		// �����ţ������ں����ֵ���43��
#define RECOG_RANGE_GESTURE			0x00000010		// ���Ʒ��ţ�36��

#define RECOG_RANGE_ADAPTATION		0x10000000		// ��ѧϰ��

// ��ĸ���ţ�����Ӣ���ܹ�52����Сд��ĸ
#define RECOG_RANGE_SYMBOL			(RECOG_RANGE_UPPERCASE | RECOG_RANGE_LOWERCASE)
#define RECOG_RANGE_ALL				(RECOG_RANGE_NUMBER | RECOG_RANGE_SYMBOL | RECOG_RANGE_INTERPUNCTION | RECOG_RANGE_GESTURE | RECOG_RANGE_ADAPTATION)

// Define Gesture
#define DG_GESTURE_NUM		36 
#define DG_ZERO				0x0000	// �ޱ���
#define DG_SPACE			0x0020	// �ո�(0x0020)
#define DG_ENTER			0x000D	// �س�(0x000D)
#define DG_BACKSPACE		0x0008	// ��ɾ(0x0008)
#define DG_DELETE			0x001E	// ɾ��(0x001E)

typedef enum
{
	HWR_LANGUAGE_BELORUSSIAN,		// �׶���˹��
	HWR_LANGUAGE_BULGARIAN,			// ����������
	HWR_LANGUAGE_CROATIAN,			// ���޵�����
	HWR_LANGUAGE_CZECHISH,			// �ݿ���
	HWR_LANGUAGE_DENISH,			// ������
	HWR_LANGUAGE_DUTCH,				// ������
	HWR_LANGUAGE_ENGLISH,			// Ӣ��
	HWR_LANGUAGE_ESTONIAN,			// ��ɳ������
	HWR_LANGUAGE_FINNISH,			// ������
	HWR_LANGUAGE_FRENCH,			// ����
	HWR_LANGUAGE_GERMAN,			// ����
	HWR_LANGUAGE_GREEK,				// ϣ����
	HWR_LANGUAGE_HUNGARIAN,			// ��������
	HWR_LANGUAGE_IRISH,				// ��������
	HWR_LANGUAGE_ITALIAN,			// �������
	HWR_LANGUAGE_LATVIAN,			// ����ά����
	HWR_LANGUAGE_LITHUANIAN,		// ��������
	HWR_LANGUAGE_LUXEMBURG,			// ¬ɭ����
	HWR_LANGUAGE_MALTESE,			// �������
	HWR_LANGUAGE_NORWEGIAN,			// Ų����
	HWR_LANGUAGE_POLISH,			// ������
	HWR_LANGUAGE_PORTUGUESE,		// ��������
	HWR_LANGUAGE_ROMANIAN,			// ����������
	HWR_LANGUAGE_RUSSIAN,			// ����˹��
	HWR_LANGUAGE_SLOVAKIAN,			// ˹�工����
	HWR_LANGUAGE_SLOVENIAN,			// ˹����������
	HWR_LANGUAGE_SPANISH,			// ��������
	HWR_LANGUAGE_SWEDISH,			// �����
	HWR_LANGUAGE_TURKISH,			// ��������
	HWR_LANGUAGE_UKRAINIAN,			// �ڿ�����

	HWR_LANGUAGE_INDONESIAN,		// ӡ����������
	HWR_LANGUAGE_MALAYSIAN,			// ����������
	HWR_LANGUAGE_VIETNAMESE,		// Խ����

	HWR_LANGUAGE_AFRIKAANS,			// �Ϸ�Ӣ��
	
	HWR_LANGUAGE_ARABIC,			// ��������
	HWR_LANGUAGE_UYGHUR				// ά�����
} HWRLanguage;

typedef enum 
{
	PARAM_CANDNUMB,			// 1~100���Ƽ�ʹ��10��ȱʡ��10
	PARAM_RECORANG,			// RECOG_RANGE_xxx����������ȱʡ��GB������������ȱʡ��SYMBOL
	PARAM_DISPCODE,			// DP_xxx,ȱʡ�ǲ��䣬������ֻ�Ժ���������Ч
	PARAM_FULLHALF,			// FH_xxx��ȱʡ��ȫ�ǣ�������ֻ�Ժ���������Ч
	PARAM_DEFGESTURE,		// ����36ά������36��WORD������36��д�����б��ƶ���
	PARAM_SPEEDUP,			// ���Ի�ø�����ٶ�,���鲻Ҫ���ô˲���,ȱʡ�ǲ����ã�������ֻ�Ժ���������Ч
	PARAM_LANGUAGE			// ���õ�ǰ�������ԣ�ȱʡ��Ӣ�ģ�������ֻ�Զ�����������Ч
} HZPARAM;

HZRECOGAPI HZ_VOID HZSetParam_Multi(HZPARAM nParam, HZ_UINT32 dwValue);
HZRECOGAPI HZ_UINT32 HZGetParam_Multi(HZPARAM nParam);

#ifdef __cplusplus
}
#endif 



