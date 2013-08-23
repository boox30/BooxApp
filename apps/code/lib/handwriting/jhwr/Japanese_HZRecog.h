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
HZRECOGAPI HZ_BOOL  HZInitCharacterRecognition(HZ_VOID* pPointer);

// Recognition Kernel Exit
// call this function before using other functions handwritting recognition kernel no more
// Return value :	TRUE	success
//					FALSE	fail
HZRECOGAPI HZ_BOOL  HZExitCharacterRecognition(HZ_VOID);

// Main Recognition Function
// Return value is recognition result number, its valid value are [-1, MAX_CANDIDATE_NUM]
// pnStrokeBuffer is handwritting buffer of Chinese character.
// (x, y) make up one point, and x and y are short type, their valid value are from -1 to 32767.
// (-1, 0) is end flag of stroke, (-1, -1) is end flag of character.
// pwResultBuffer is recognition result buffer, we recommend its size are MAX_CANDIDATE_NUM+1
// uMatchRange is one of four kinds of match_range or combination of them. Note: it is not null(zero)
// moreover, uMatchRange may include ADAPTATION_MATCH_RANGE only after calling 'HZInitAdaptation'
HZRECOGAPI HZ_INT32  HZCharacterRecognize(HZ_INT16* pnStrokeBuffer, HZ_WORD* pwResult);

#define RECOG_RANGE_NUMBER			0x00000001		// ���֣�0~9ʮ������
#define RECOG_RANGE_UPPERCASE		0x00000002		// ��д��ĸ��26��
#define RECOG_RANGE_LOWERCASE		0x00000004		// Сд��ĸ��26��
#define RECOG_RANGE_INTERPUNCTION	0x00000008		// �����ţ�31��
#define RECOG_RANGE_GESTURE			0x00000010		// ���Ʒ��ţ�36��
#define RECOG_RANGE_0208_LEVEL1		0x00000100		// JIS 0208 level 1, 2965��
#define RECOG_RANGE_0208_LEVEL2		0x00000200		// JIS 0208 level 2, 3390��
#define RECOG_RANGE_0212			0x00000400		// JIS 0212, 5801��
#define RECOG_RANGE_HIRAGANA		0x00010000		// hiragana, 83��
#define RECOG_RANGE_KATAKANA		0x00020000		// katakana, 86��
#define RECOG_RANGE_ADAPTATION		0x10000000		// ��ѧϰ��

// ��ĸ���ţ��ܹ�52����Сд��ĸ
#define RECOG_RANGE_SYMBOL			(RECOG_RANGE_UPPERCASE | RECOG_RANGE_LOWERCASE)
// JIS 0208���ܹ�6355������
#define RECOG_RANGE_0208			(RECOG_RANGE_0208_LEVEL1 | RECOG_RANGE_0208_LEVEL2)
// JIS���֣��ܹ�12156������
#define RECOG_RANGE_JIS				(RECOG_RANGE_0208 | RECOG_RANGE_0212)

// Define Gesture
#define DG_GESTURE_NUM		36 
#define DG_ZERO				0x0000	// �ޱ���
#define DG_SPACE			0x0020	// �ո�(0x0020)
#define DG_ENTER			0x000D	// �س�(0x000D)
#define DG_BACKSPACE		0x0008	// ��ɾ(0x0008)
#define DG_DELETE			0x001E	// ɾ��(0x001E)

typedef enum 
{
	PARAM_CANDNUMB = 0,		// 1~100���Ƽ�ʹ��10��ȱʡ��10
	PARAM_RECORANG = 1,		// RECOG_RANGE_xxx��ȱʡ��JIS
	PARAM_DEFGESTURE = 4,	// ����36ά������36��WORD������36��д�����б��ƶ���
	PARAM_SPEEDUP = 5,		// ���Ի�ø�����ٶ�,ȱʡ������
} HZPARAM;

HZRECOGAPI HZ_VOID HZSetParam(HZPARAM nParam, HZ_UINT32 dwValue);
HZRECOGAPI HZ_UINT32 HZGetParam(HZPARAM nParam);

#ifdef __cplusplus
}
#endif 



