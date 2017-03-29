/*  ScreenShot Uploader- Saves your screenshots and uploads to a image-hosting site
    Copyright (C) 2009  manizzle(Murtaza Munaim)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <windows.h>

int pixdump(void)
{

	//definitions

	INPUT ss[1]; 
	UINT format,bytes; 
	HBITMAP hbmp;
	LPCSTR fpath= "C:\\test\\test.bmp"; 
	DWORD error;
	HDC hdc;
	BITMAPINFO bmpinfo;
	BITMAPFILEHEADER bmpflhdr;
	LPVOID bmpbuf;
	FILE* fp;
	

	//stuff to simulate clicking the Prt Scrn button
	ss[0].type=INPUT_KEYBOARD;
	ss[0].ki.wVk=0x2C;
	ss[0].ki.wScan=MapVirtualKey(0x2C,0);
	ss[0].ki.dwFlags=KEYEVENTF_SCANCODE;
	ss[0].ki.time=0;
	ss[0].ki.dwExtraInfo=0;
	
	if(SendInput(1,ss,sizeof(INPUT)) !=1)
	{
		printf("error/keyboard bind blocked by other process"); 
		return 0;
	}

	//stuff to dump the pix	
	//to open the clipboard
	if( OpenClipboard(NULL) < 0)
	{
		error=GetLastError(); 
		printf("opening clipboard failed error: %u,",error); 
		return 0;
	}

	//to find the correct present file format of the clipboard
	format=EnumClipboardFormats(0); 
	while(format)
	{
		printf("format is: %u\n",format); 
		
		if(!IsClipboardFormatAvailable(format))
			printf("file format: %u is not present\n",format);
			
		if(IsClipboardFormatAvailable(format))
		{
			printf("file format: %u is present\n",format);
			hbmp=(HBITMAP)GetClipboardData(format); break;
		}
			
		format=EnumClipboardFormats(format);
		
	}

	if(!hbmp)
	{
		printf("failed to get the clipboard data");
		return 0;
	}
	
	hdc=GetDC(NULL);
	
	//trying to get the size of the bitmap image and set things
	bmpinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	GetDIBits(hdc,hbmp,0,0,NULL,&bmpinfo,DIB_RGB_COLORS);
	bmpinfo.bmiHeader.biSizeImage=(DWORD)((bmpinfo.bmiHeader.biWidth)*abs(bmpinfo.bmiHeader.biHeight)*((bmpinfo.bmiHeader.biBitCount)/8));
	bmpinfo.bmiHeader.biCompression=BI_RGB;
	
	//filling out the bitmapfileheader struct
	bmpflhdr.bfType=0x4D42;
	bmpflhdr.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO)+bmpinfo.bmiHeader.biSizeImage;
	bmpflhdr.bfReserved1=0;
	bmpflhdr.bfReserved2=0;
	bmpflhdr.bfOffBits=sizeof(BITMAPINFO)+sizeof(BITMAPFILEHEADER);
	
	//allocate enuff space for the bitmap data
	bmpbuf=malloc(bmpinfo.bmiHeader.biSizeImage);
	
	//write bitmap image data to a void*(bmpbuf)
	GetDIBits(hdc,hbmp,0,bmpinfo.bmiHeader.biHeight,bmpbuf,&bmpinfo,DIB_RGB_COLORS);
	
	//open and create file
	fp=fopen(fpath,"wb");
	
	//write the data to the file in order
	
	//first the file header
	bytes=fwrite(&bmpflhdr,sizeof(bmpflhdr),1,fp);
	printf("the file header was %u bytes\n",bytes);
	
	//next the bitmap info
	bytes=fwrite(&bmpinfo,sizeof(bmpinfo),1,fp);
	printf("the info struct was %u bytes\n",bytes);
	
	//finally the main bitmap data
	bytes=fwrite(bmpbuf,bmpinfo.bmiHeader.biSizeImage,1,fp);
	printf("the bitmap was %u bytes\n",bytes);
	
	//cleanup
	if(!CloseClipboard())
	{
		printf("closing clipboard failed");
		return 0;
	}
	fclose(fp);
	ReleaseDC(NULL,hdc);
	free(bmpbuf);

	//return if all went well..
	return 1;
}

int main(void)
{
	if(pixdump()) 
		printf("it fuckin worked!!!\n");
	printf("\n\n");
	system("pause");
}