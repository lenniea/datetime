#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[])
{
	HANDLE hFindFile;
	WIN32_FIND_DATA fd;
	int count = 0;

	fprintf(stderr, "datetime v1.0.0 (12-Mar-2005)\n");
	if (argc < 2)
	{
		fprintf(stderr, "usage: datetime filespec\n");
	}
	else
	{
		hFindFile = FindFirstFile(argv[1], &fd);
		if (hFindFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				LPCTSTR filename = fd.cFileName;

				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					fprintf(stdout, "Directory: %s\n", filename);
				}
				else
				{
					HANDLE hFile = CreateFile(filename, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE)
					{

						fprintf(stdout, "File: %s\n", filename);
						if (isdigit(filename[0]) && isdigit(filename[1]) && isdigit(filename[2]) && isdigit(filename[3]) && isdigit(filename[4]) && isdigit(filename[5]))
						{
							SYSTEMTIME st;
							FILETIME ft,uft;

							memset(&st, 0, sizeof(st));
							st.wYear =  (filename[0] - '0') * 1000 + (filename[1] - '0') * 100 + (filename[2] - '0') * 10 + (filename[3] - '0') * 1;
							st.wMonth = (filename[4] - '0') * 10 + (filename[5] - '0') * 1;
							st.wDay = 1;

							st.wHour = 1;		// 1:00a
							st.wMinute = 0;
							SystemTimeToFileTime(&st, &ft);
							LocalFileTimeToFileTime(&ft, &uft);

							if (SetFileTime(hFile, &uft, &uft, &uft))
								++count;
						}
					}
					CloseHandle(hFile);
				}
			}
			while (FindNextFile(hFindFile, &fd));
			FindClose(hFindFile);
		}
		fprintf(stderr, "%u Files processed\n", count);
	}
	return 0;
}
