#include <stdio.h>
#include <windows.h>

int dir_stamp(const char* srcExt, const char* dstExt)
{

	HANDLE hSrcFind;
	int count = 0;
	TCHAR pathname[_MAX_PATH];
	WIN32_FIND_DATA fdSrc;
	wsprintf(pathname, "*.%s", srcExt);

	hSrcFind = FindFirstFile(pathname, &fdSrc);
	if (hSrcFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			SYSTEMTIME sysTime;
			FILETIME createTime, accessTime, modTime, localTime;
			
			if (!(fdSrc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) /* only process files */
			{
				WIN32_FIND_DATA fdDst;
				HANDLE hFile;
				HANDLE hDstFind;
				LPTSTR pExt;

				/* Find "matching" filename */
				strcpy(pathname, fdSrc.cFileName);
				pExt = strrchr(pathname, '.');
				if (pExt != NULL)
				{
					/* Overwrite extension with "*" */
					sprintf(pExt, ".%s", dstExt);
				}
				hDstFind = FindFirstFile(pathname, &fdDst);
				if (hDstFind != INVALID_HANDLE_VALUE)
				{
					/* Copy file create, access & modification timestamp */
					hFile = CreateFile(fdSrc.cFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE)
					{
						/* Read source file timestamps */
						GetFileTime(hFile, &createTime, &accessTime, &modTime);
						CloseHandle(hFile);

						hFile = CreateFile(pathname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						if (hFile != INVALID_HANDLE_VALUE)
						{
							/* Write timestamps to destination file */
							BOOL bOK = SetFileTime(hFile, &createTime, &accessTime, &modTime);
							CloseHandle(hFile);

							FileTimeToLocalFileTime(&modTime, &localTime);
							FileTimeToSystemTime(&localTime, &sysTime);
                            fprintf(stdout, "Src:%s Dst:%s %04u-%02u-%02u %02u:%02u:%02u=%s\n", fdSrc.cFileName, fdDst.cFileName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, bOK ? "OK" : "ERROR");
							++count;
						}
					}
					FindClose(hDstFind);
				}
			}
		}
		while (FindNextFile(hSrcFind, &fdSrc));
		FindClose(hSrcFind);
	}
	fprintf(stderr, "%u Files processed\n", count);
	return count;
}

int main(int argc, char* argv[])
{
	int result;
	fprintf(stderr, "dirstamp v1.01 (12-Nov-2012)\n");
	if (argc != 3)
	{
		fprintf(stderr, "usage: dirstamp srcext dstext\n");
		result = -1;
	}
	else
	{
		result = dir_stamp(argv[1], argv[2]);
	}
	return result;
}
