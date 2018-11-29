//�ù�����ѹ���ļ�(C���ԣ�
//���ù�������������ѹ��������������£�
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <conio.h>

struct head 
{
	unsigned char b;        //��¼�ַ��������е�λ��
	long count;             //�ַ�����Ƶ�ʣ�Ȩֵ�� 
	long parent,lch,rch;    //�����������ָ�����
	char bits[256];         //����洢���������������
} 
header[512],tmp;

/*ѹ��*/
void compress() 
{
	char filename[255],outputfile[255],buf[512]; 
	unsigned char c; 
	long i,j,m,n,f; 
	long min1,pt1,flength,length1,length2; 
	double div;
	FILE *ifp,*ofp;

	printf("\t����������Ҫѹ�����ļ�������Ҫ�����ļ���׺������"); 
	gets(filename); 
	ifp=fopen(filename,"rb"); 
	if(ifp==NULL) 
	{
		printf("\n\t�ļ���ʧ����!\n\n"); 
		return; 
	}
	printf("\t��������ѹ������ļ��������������ļ���׺������"); 
	gets(outputfile); 
	ofp=fopen(strcat(outputfile,".txt"),"wb"); 
	if(ofp==NULL) 
	{
		printf("\n\tѹ���ļ�ʧ����!\n\n"); 
		return; 
	}
	flength=0; 
	while(!feof(ifp)) 
	{
		fread(&c,1,1,ifp); 
		header[c].count++;    //�ַ��ظ�����Ƶ��+1
		flength++;            //�ַ�����ԭ�ļ�����+1
	}
	flength--; 
	length1=flength;          //ԭ�ļ�����������ѹ���ʵķ�ĸ
	header[c].count--; 
	for(i=0;i<512;i++) 
	{
		if(header[i].count!=0) header[i].b=(unsigned char)i; 
		/*��ÿ����������ֵ�����Ӧ��ASCII������һά����header[i]�У�
		�ұ�����е��±��ASCII������˳���Ź�ϵ*/
		else header[i].b=0; 
		header[i].parent=-1;header[i].lch=header[i].rch=-1;    //�Խ����г�ʼ��
	} 
	for(i=0;i<256;i++)    //����Ƶ�ʣ�Ȩֵ����С���Խ���������ѡ���С�Ľ�����
	{
		for(j=i+1;j<256;j++)
		{
			if(header[i].count<header[j].count)
			{
				tmp=header[i];
				header[i]=header[j]; 
				header[j]=tmp; 
			} 
		} 
	}
	for(i=0;i<256;i++) if(header[i].count==0) break; 
	n=i;       //�ⲿҶ�ӽ����Ϊn��ʱ���ڲ������Ϊn-1������������������Ҫ�Ľ����Ϊ2*n-1.
	m=2*n-1;
	for(i=n;i<m;i++)   //������������
	{
		min1=999999999;   //Ԥ������Ȩֵ���������ֵ�������
		for(j=0;j<i;j++) 
		{
			if(header[j].parent!=-1) continue;    
			//parent!=-1˵���ý���Ѵ��ڹ��������У�����ѭ������ѡ���½��*/
			if(min1>header[j].count) 
			{
				pt1=j; 
				min1=header[j].count; 
				continue; 
			} 
		}
		header[i].count=header[pt1].count; 
		header[pt1].parent=i;   //����parent��ֵ����������ȷ�����н��֮��Ĺ�ϵ
		header[i].lch=pt1;   //�������֧Ȩֵ��С
		min1=999999999;   
		for(j=0;j<i;j++) 
		{
			if(header[j].parent!=-1) continue; 
			if(min1>header[j].count) 
			{
				pt1=j; 
				min1=header[j].count; 
				continue; 
			} 
		}
		header[i].count+=header[pt1].count; 
		header[i].rch=pt1;   //�����ҷ�֧Ȩֵ��С
		header[pt1].parent=i; 
	}
	for(i=0;i<n;i++)   //���������ظ�ǰ׺����
	{
		f=i; 
		header[i].bits[0]=0;   //��������0   
		while(header[f].parent!=-1) 
		{
			j=f; 
			f=header[f].parent; 
			if(header[f].lch==j)   //�����֧����0
			{
				j=strlen(header[i].bits); 
				memmove(header[i].bits+1,header[i].bits,j+1);
				//���δ洢���ӡ�0����1������
				header[i].bits[0]='0'; 
			}
			else   //���ҷ�֧����1
			{
				j=strlen(header[i].bits); 
				memmove(header[i].bits+1,header[i].bits,j+1); 
				header[i].bits[0]='1'; 
			} 
		} 
	}
	fseek(ifp,0,SEEK_SET);   //���ļ���ʼλ����ǰ�ƶ�0�ֽڣ�����λ���ļ���ʼλ��
	fwrite(&flength,sizeof(int),1,ofp);
	/*����������д���ļ����У�����flengthָ����д������ݵ�ַ��
	�ܹ�д����ַ����Բ���size*int������������ʵ��д���int��Ŀ1*/ 
	fseek(ofp,8,SEEK_SET); 
	buf[0]=0;   //���建����,���Ķ����Ʊ�ʾ00000000
	f=0; 
	pt1=8; 
	/*����ԭ�ļ���һ���ַ���"A"��8λ2����Ϊ01000001�������Ϊ0110ʶ������һ��'0'��
	��ô���ǾͿ��Խ�������һλ��������ûʲô�仯����һ����'1'��Ӧ��|1�����00000001 
	ͬ��4λ�����꣬Ӧ����00000110�������ֽ��е�8λ��û��ȫ�����꣬����Ӧ�ü�������һ���ַ���
	���ݱ�������ƴ��ʣ�µ�4λ������ַ��ı��벻��4λ����Ҫ������һ���ַ���
	����ַ����볬��4λ����ô���ǽ���ʣ�µ�λ��Ϣƴ�ӵ�һ���µ��ֽ���*/
	while(!feof(ifp)) 
	{
		c=fgetc(ifp); 
		f++; 
		for(i=0;i<n;i++) 
		{
			if(c==header[i].b) break; 
		}
		strcat(buf,header[i].bits); 
		j=strlen(buf);
		c=0; 
		while(j>=8)   //�Թ���������λ��������ѹ���洢
		{
			for(i=0;i<8;i++) 
			{
				if(buf[i]=='1') c=(c<<1)|1; 
				else c=c<<1; 
			}
			fwrite(&c,1,1,ofp); 
			pt1++;   //ͳ��ѹ�����ļ��ĳ���
			strcpy(buf,buf+8);   //һ���ֽ�һ���ֽ�ƴ��
			j=strlen(buf); 
		}
		if(f==flength) break; 
	}
	if(j>0)    //�Թ���������λ��������ѹ���洢
	{
		strcat(buf,"00000000"); 
		for(i=0;i<8;i++) 
		{
			if(buf[i]=='1') c=(c<<1)|1; 
			else c=c<<1; 
		}
		fwrite(&c,1,1,ofp); 
		pt1++; 
	}
	fseek(ofp,4,SEEK_SET); 
	fwrite(&pt1,sizeof(long),1,ofp); 
	fseek(ofp,pt1,SEEK_SET); 
	fwrite(&n,sizeof(long),1,ofp); 
	for(i=0;i<n;i++) 
	{
		fwrite(&(header[i].b),1,1,ofp); 
		c=strlen(header[i].bits); 
		fwrite(&c,1,1,ofp); 
		j=strlen(header[i].bits); 
		if(j%8!=0)   //���洢��λ������8�ı�������0   
		{
			for(f=j%8;f<8;f++) 
				strcat(header[i].bits,"0"); 
		}
		while(header[i].bits[0]!=0) 
		{
			c=0; 
			for(j=0;j<8;j++)   //�ַ�����Ч�洢������8λ�������Чλ������ʵ�����ַ����������
			{
				if(header[i].bits[j]=='1') c=(c<<1)|1;   //|1���ı�ԭλ���ϵġ�0����1��ֵ
				else c=c<<1; 
			}
			strcpy(header[i].bits,header[i].bits+8);   //���ַ��ı��밴ԭ�ȴ洢˳������
			fwrite(&c,1,1,ofp); 
		} 
	} 
	length2=pt1--;
	div=((double)length1-(double)length2)/(double)length1;   //�����ļ���ѹ����
	fclose(ifp); 
	fclose(ofp); 
	printf("\n\tѹ���ļ��ɹ�!\n"); 
	printf("\tѹ����Ϊ %f%%\n\n",div*100); 
	return; 
}
/*��ѹ��*/
void uncompress() 
{
	char filename[255],outputfile[255],buf[255],bx[255]; 
	unsigned char c; 
	long i,j,m,n,f,p,l; 
	long flength; 
	FILE *ifp,*ofp; 
	printf("\t����������Ҫ��ѹ�����ļ�(���������׺��)��"); 
	gets(filename); 
	ifp=fopen(strcat(filename,".txt"),"rb"); 
	if(ifp==NULL) 
	{
		printf("\n\t�ļ���ʧ��!\n"); 
		return; 
	}
	printf("\t���������ѹ������ļ�������ָ���ļ���׺������"); 
	gets(outputfile); 
	ofp=fopen(outputfile,"wb"); 
	if(ofp==NULL) 
	{
		printf("\n\t��ѹ���ļ�ʧ��!\n"); 
		return; 
	}
	fread(&flength,sizeof(long),1,ifp);   //��ȡԭ�ļ����ȣ����ļ����ж�λ
	fread(&f,sizeof(long),1,ifp); 
	fseek(ifp,f,SEEK_SET); 
	fread(&n,sizeof(long),1,ifp); 
	for(i=0;i<n;i++) 
	{
		fread(&header[i].b,1,1,ifp); 
		fread(&c,1,1,ifp); 
		p=(long)c;   //��ȡԭ�ļ��ַ���Ȩֵ
		header[i].count=p; 
		header[i].bits[0]=0; 
		if(p%8>0) m=p/8+1; 
		else m=p/8; 
		for(j=0;j<m;j++) 
		{
			fread(&c,1,1,ifp); 
			f=c; 
			itoa(f,buf,2);   //��fת��Ϊ�����Ʊ�ʾ���ַ���
			f=strlen(buf); 
			for(l=8;l>f;l--) 
			{
				strcat(header[i].bits,"0"); 
			}
			strcat(header[i].bits,buf); 
		} 
		header[i].bits[p]=0; 
	} 
	for(i=0;i<n;i++)   //���ݹ���������ĳ��̣��Խ���������
	{
		for(j=i+1;j<n;j++) 
		{
			if(strlen(header[i].bits)>strlen(header[j].bits)) 
			{
				tmp=header[i]; 
				header[i]=header[j]; 
				header[j]=tmp; 
			} 
		} 
	} 
	p=strlen(header[n-1].bits); 
	fseek(ifp,8,SEEK_SET); 
	m=0; 
	bx[0]=0; 
	while(1)    //ͨ������������ĳ��̣����ν��룬��ԭ����λ�洢��ԭ���ֽڴ洢
	{
		while(strlen(bx)<(unsigned int)p) 
		{
			fread(&c,1,1,ifp); 
			f=c; 
			itoa(f,buf,2); 
			f=strlen(buf); 
			for(l=8;l>f;l--) //�ڵ��ֽ��ڶ���Ӧλ�ò�0
			{
				strcat(bx,"0"); 
			}
			strcat(bx,buf); 
		}
		for(i=0;i<n;i++) 
		{
			if(memcmp(header[i].bits,bx,header[i].count)==0) break; 
		}
		strcpy(bx,bx+header[i].count);   /*��ѹ���ļ��еİ�λ�洢��ԭ�����ֽڴ洢�ַ���
		�ַ�λ�ò��ı�*/
		c=header[i].b; 
		fwrite(&c,1,1,ofp); 
		m++;   //ͳ�ƽ�ѹ�����ļ��ĳ���
		if(m==flength) break;   //flength��ԭ�ļ�����
	} 
	fclose(ifp); 
	fclose(ofp); 
	printf("\n\t��ѹ���ļ��ɹ�!\n"); 
	if(m==flength)   //�Խ�ѹ�����ļ���ԭ�ļ���ͬ�ԱȽϽ����жϣ������ļ���С��
		printf("\t��ѹ���ļ���ԭ�ļ���ͬ!\n\n"); 
	else printf("\t��ѹ���ļ���ԭ�ļ���ͬ!\n\n");
	return; 
}
/*������*/
int main() 
{
	int c; 
	while(1)   //�˵�������
	{
		printf("\t _________________________________________________________\n");
		printf("\n");
		printf("\t         * ����huffman������ı��ļ�����ѹ�����ѹ *      \n");
		printf("\t _________________________________________________________\n");   
		printf("\t _________________________________________________________\n");
		printf("\t                               			                \n");   
		printf("\t 1.ѹ��                 			                        \n");   
		printf("\t 2.��ѹ��                                			    \n");   
		printf("\t 0.�˳�                                  		        \n");
		printf("\t__________________________________________________________\n");
	 
		printf("\n");
		do   //���û���������ݴ���
		{
			printf("\n\t*��ѡ����Ӧ����(0-2):");     
			c=getch(); 
			printf("%c\n",c); 
			if(c!='0' && c!='1' && c!='2')
			{ 
				printf("\t@_@��������������0~2֮�䣡\n");
				printf("\t��������һ�飡\n");
			}
		}while(c!='0' && c!='1' && c!='2'); 
		if(c=='1') compress();          //����ѹ���Ӻ���
		else if(c=='2') uncompress();   //���ý�ѹ���Ӻ���
		else 
		{
			printf("\t��ӭ���ٴ�ʹ�øù���^_^\n"); 
			exit(0);                    //�˳��ù���
		}
		system("pause");   //���������
		system("cls");     //����
	}
	return 0;
}
