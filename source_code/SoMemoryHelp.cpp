//----------------------------------------------------------------
#include "SoMemoryHelp.h"
#include <string> //for memcpy()
//----------------------------------------------------------------
void* SoTinyMemCpy(void* dest, const void* src, const int len)
{
	if (len > 64)
	{
		return memcpy(dest, src, len);
	}

	register unsigned char* dd = (unsigned char*)dest + len;
	register const unsigned char* ss = (const unsigned char*)src + len;
	switch (len) 
	{
		case 68:    *((int*)(dd - 68)) = *((int*)(ss - 68));
		case 64:    *((int*)(dd - 64)) = *((int*)(ss - 64));
		case 60:    *((int*)(dd - 60)) = *((int*)(ss - 60));
		case 56:    *((int*)(dd - 56)) = *((int*)(ss - 56));
		case 52:    *((int*)(dd - 52)) = *((int*)(ss - 52));
		case 48:    *((int*)(dd - 48)) = *((int*)(ss - 48));
		case 44:    *((int*)(dd - 44)) = *((int*)(ss - 44));
		case 40:    *((int*)(dd - 40)) = *((int*)(ss - 40));
		case 36:    *((int*)(dd - 36)) = *((int*)(ss - 36));
		case 32:    *((int*)(dd - 32)) = *((int*)(ss - 32));
		case 28:    *((int*)(dd - 28)) = *((int*)(ss - 28));
		case 24:    *((int*)(dd - 24)) = *((int*)(ss - 24));
		case 20:    *((int*)(dd - 20)) = *((int*)(ss - 20));
		case 16:    *((int*)(dd - 16)) = *((int*)(ss - 16));
		case 12:    *((int*)(dd - 12)) = *((int*)(ss - 12));
		case  8:    *((int*)(dd - 8)) = *((int*)(ss - 8));
		case  4:    *((int*)(dd - 4)) = *((int*)(ss - 4));
			break;
		case 67:    *((int*)(dd - 67)) = *((int*)(ss - 67));
		case 63:    *((int*)(dd - 63)) = *((int*)(ss - 63));
		case 59:    *((int*)(dd - 59)) = *((int*)(ss - 59));
		case 55:    *((int*)(dd - 55)) = *((int*)(ss - 55));
		case 51:    *((int*)(dd - 51)) = *((int*)(ss - 51));
		case 47:    *((int*)(dd - 47)) = *((int*)(ss - 47));
		case 43:    *((int*)(dd - 43)) = *((int*)(ss - 43));
		case 39:    *((int*)(dd - 39)) = *((int*)(ss - 39));
		case 35:    *((int*)(dd - 35)) = *((int*)(ss - 35));
		case 31:    *((int*)(dd - 31)) = *((int*)(ss - 31));
		case 27:    *((int*)(dd - 27)) = *((int*)(ss - 27));
		case 23:    *((int*)(dd - 23)) = *((int*)(ss - 23));
		case 19:    *((int*)(dd - 19)) = *((int*)(ss - 19));
		case 15:    *((int*)(dd - 15)) = *((int*)(ss - 15));
		case 11:    *((int*)(dd - 11)) = *((int*)(ss - 11));
		case  7:    *((int*)(dd - 7)) = *((int*)(ss - 7));
			*((int*)(dd - 4)) = *((int*)(ss - 4));
			break;
		case  3:    *((short*)(dd - 3)) = *((short*)(ss - 3));
			dd[-1] = ss[-1];
			break;
		case 66:    *((int*)(dd - 66)) = *((int*)(ss - 66));
		case 62:    *((int*)(dd - 62)) = *((int*)(ss - 62));
		case 58:    *((int*)(dd - 58)) = *((int*)(ss - 58));
		case 54:    *((int*)(dd - 54)) = *((int*)(ss - 54));
		case 50:    *((int*)(dd - 50)) = *((int*)(ss - 50));
		case 46:    *((int*)(dd - 46)) = *((int*)(ss - 46));
		case 42:    *((int*)(dd - 42)) = *((int*)(ss - 42));
		case 38:    *((int*)(dd - 38)) = *((int*)(ss - 38));
		case 34:    *((int*)(dd - 34)) = *((int*)(ss - 34));
		case 30:    *((int*)(dd - 30)) = *((int*)(ss - 30));
		case 26:    *((int*)(dd - 26)) = *((int*)(ss - 26));
		case 22:    *((int*)(dd - 22)) = *((int*)(ss - 22));
		case 18:    *((int*)(dd - 18)) = *((int*)(ss - 18));
		case 14:    *((int*)(dd - 14)) = *((int*)(ss - 14));
		case 10:    *((int*)(dd - 10)) = *((int*)(ss - 10));
		case  6:    *((int*)(dd - 6)) = *((int*)(ss - 6));
		case  2:    *((short*)(dd - 2)) = *((short*)(ss - 2));
			break;
		case 65:    *((int*)(dd - 65)) = *((int*)(ss - 65));
		case 61:    *((int*)(dd - 61)) = *((int*)(ss - 61));
		case 57:    *((int*)(dd - 57)) = *((int*)(ss - 57));
		case 53:    *((int*)(dd - 53)) = *((int*)(ss - 53));
		case 49:    *((int*)(dd - 49)) = *((int*)(ss - 49));
		case 45:    *((int*)(dd - 45)) = *((int*)(ss - 45));
		case 41:    *((int*)(dd - 41)) = *((int*)(ss - 41));
		case 37:    *((int*)(dd - 37)) = *((int*)(ss - 37));
		case 33:    *((int*)(dd - 33)) = *((int*)(ss - 33));
		case 29:    *((int*)(dd - 29)) = *((int*)(ss - 29));
		case 25:    *((int*)(dd - 25)) = *((int*)(ss - 25));
		case 21:    *((int*)(dd - 21)) = *((int*)(ss - 21));
		case 17:    *((int*)(dd - 17)) = *((int*)(ss - 17));
		case 13:    *((int*)(dd - 13)) = *((int*)(ss - 13));
		case  9:    *((int*)(dd - 9)) = *((int*)(ss - 9));
		case  5:    *((int*)(dd - 5)) = *((int*)(ss - 5));
		case  1:    dd[-1] = ss[-1];
			break;
		case 0:
		default:
			break;
	}
	return dd;
}
//----------------------------------------------------------------
