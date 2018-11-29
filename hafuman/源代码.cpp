//用哈夫曼压缩文件(C语言）
//利用哈夫曼编码制作压缩软件，内容如下：
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <conio.h>

struct head 
{
	unsigned char b;        //记录字符在数组中的位置
	long count;             //字符出现频率（权值） 
	long parent,lch,rch;    //定义哈夫曼树指针变量
	char bits[256];         //定义存储哈夫曼编码的数组
} 
header[512],tmp;

/*压缩*/
void compress() 
{
	char filename[255],outputfile[255],buf[512]; 
	unsigned char c; 
	long i,j,m,n,f; 
	long min1,pt1,flength,length1,length2; 
	double div;
	FILE *ifp,*ofp;

	printf("\t请您输入需要压缩的文件名（需要输入文件后缀名）："); 
	gets(filename); 
	ifp=fopen(filename,"rb"); 
	if(ifp==NULL) 
	{
		printf("\n\t文件打开失败啦!\n\n"); 
		return; 
	}
	printf("\t请您输入压缩后的文件名（无需输入文件后缀名）："); 
	gets(outputfile); 
	ofp=fopen(strcat(outputfile,".txt"),"wb"); 
	if(ofp==NULL) 
	{
		printf("\n\t压缩文件失败啦!\n\n"); 
		return; 
	}
	flength=0; 
	while(!feof(ifp)) 
	{
		fread(&c,1,1,ifp); 
		header[c].count++;    //字符重复出现频率+1
		flength++;            //字符出现原文件长度+1
	}
	flength--; 
	length1=flength;          //原文件长度用作求压缩率的分母
	header[c].count--; 
	for(i=0;i<512;i++) 
	{
		if(header[i].count!=0) header[i].b=(unsigned char)i; 
		/*将每个哈夫曼码值及其对应的ASCII码存放在一维数组header[i]中，
		且编码表中的下标和ASCII码满足顺序存放关系*/
		else header[i].b=0; 
		header[i].parent=-1;header[i].lch=header[i].rch=-1;    //对结点进行初始化
	} 
	for(i=0;i<256;i++)    //根据频率（权值）大小，对结点进行排序，选择较小的结点进树
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
	n=i;       //外部叶子结点数为n个时，内部结点数为n-1，整个哈夫曼树的需要的结点数为2*n-1.
	m=2*n-1;
	for(i=n;i<m;i++)   //构建哈夫曼树
	{
		min1=999999999;   //预设的最大权值，即结点出现的最大次数
		for(j=0;j<i;j++) 
		{
			if(header[j].parent!=-1) continue;    
			//parent!=-1说明该结点已存在哈夫曼树中，跳出循环重新选择新结点*/
			if(min1>header[j].count) 
			{
				pt1=j; 
				min1=header[j].count; 
				continue; 
			} 
		}
		header[i].count=header[pt1].count; 
		header[pt1].parent=i;   //依据parent域值（结点层数）确定树中结点之间的关系
		header[i].lch=pt1;   //计算左分支权值大小
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
		header[i].rch=pt1;   //计算右分支权值大小
		header[pt1].parent=i; 
	}
	for(i=0;i<n;i++)   //哈夫曼无重复前缀编码
	{
		f=i; 
		header[i].bits[0]=0;   //根结点编码0   
		while(header[f].parent!=-1) 
		{
			j=f; 
			f=header[f].parent; 
			if(header[f].lch==j)   //置左分支编码0
			{
				j=strlen(header[i].bits); 
				memmove(header[i].bits+1,header[i].bits,j+1);
				//依次存储连接“0”“1”编码
				header[i].bits[0]='0'; 
			}
			else   //置右分支编码1
			{
				j=strlen(header[i].bits); 
				memmove(header[i].bits+1,header[i].bits,j+1); 
				header[i].bits[0]='1'; 
			} 
		} 
	}
	fseek(ifp,0,SEEK_SET);   //从文件开始位置向前移动0字节，即定位到文件开始位置
	fwrite(&flength,sizeof(int),1,ofp);
	/*用来将数据写入文件流中，参数flength指向欲写入的数据地址，
	总共写入的字符数以参数size*int来决定，返回实际写入的int数目1*/ 
	fseek(ofp,8,SEEK_SET); 
	buf[0]=0;   //定义缓冲区,它的二进制表示00000000
	f=0; 
	pt1=8; 
	/*假设原文件第一个字符是"A"，8位2进制为01000001，编码后为0110识别编码第一个'0'，
	那么我们就可以将其左移一位，看起来没什么变化。下一个是'1'，应该|1，结果00000001 
	同理4位都做完，应该是00000110，由于字节中的8位并没有全部用完，我们应该继续读下一个字符，
	根据编码表继续拼完剩下的4位，如果字符的编码不足4位，还要继续读一个字符，
	如果字符编码超过4位，那么我们将把剩下的位信息拼接到一个新的字节里*/
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
		while(j>=8)   //对哈夫曼编码位操作进行压缩存储
		{
			for(i=0;i<8;i++) 
			{
				if(buf[i]=='1') c=(c<<1)|1; 
				else c=c<<1; 
			}
			fwrite(&c,1,1,ofp); 
			pt1++;   //统计压缩后文件的长度
			strcpy(buf,buf+8);   //一个字节一个字节拼接
			j=strlen(buf); 
		}
		if(f==flength) break; 
	}
	if(j>0)    //对哈夫曼编码位操作进行压缩存储
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
		if(j%8!=0)   //若存储的位数不是8的倍数，则补0   
		{
			for(f=j%8;f<8;f++) 
				strcat(header[i].bits,"0"); 
		}
		while(header[i].bits[0]!=0) 
		{
			c=0; 
			for(j=0;j<8;j++)   //字符的有效存储不超过8位，则对有效位数左移实现两字符编码的连接
			{
				if(header[i].bits[j]=='1') c=(c<<1)|1;   //|1不改变原位置上的“0”“1”值
				else c=c<<1; 
			}
			strcpy(header[i].bits,header[i].bits+8);   //把字符的编码按原先存储顺序连接
			fwrite(&c,1,1,ofp); 
		} 
	} 
	length2=pt1--;
	div=((double)length1-(double)length2)/(double)length1;   //计算文件的压缩率
	fclose(ifp); 
	fclose(ofp); 
	printf("\n\t压缩文件成功!\n"); 
	printf("\t压缩率为 %f%%\n\n",div*100); 
	return; 
}
/*解压缩*/
void uncompress() 
{
	char filename[255],outputfile[255],buf[255],bx[255]; 
	unsigned char c; 
	long i,j,m,n,f,p,l; 
	long flength; 
	FILE *ifp,*ofp; 
	printf("\t请您输入需要解压缩的文件(无需输入后缀名)："); 
	gets(filename); 
	ifp=fopen(strcat(filename,".txt"),"rb"); 
	if(ifp==NULL) 
	{
		printf("\n\t文件打开失败!\n"); 
		return; 
	}
	printf("\t请您输入解压缩后的文件名（需指定文件后缀名）："); 
	gets(outputfile); 
	ofp=fopen(outputfile,"wb"); 
	if(ofp==NULL) 
	{
		printf("\n\t解压缩文件失败!\n"); 
		return; 
	}
	fread(&flength,sizeof(long),1,ifp);   //读取原文件长度，对文件进行定位
	fread(&f,sizeof(long),1,ifp); 
	fseek(ifp,f,SEEK_SET); 
	fread(&n,sizeof(long),1,ifp); 
	for(i=0;i<n;i++) 
	{
		fread(&header[i].b,1,1,ifp); 
		fread(&c,1,1,ifp); 
		p=(long)c;   //读取原文件字符的权值
		header[i].count=p; 
		header[i].bits[0]=0; 
		if(p%8>0) m=p/8+1; 
		else m=p/8; 
		for(j=0;j<m;j++) 
		{
			fread(&c,1,1,ifp); 
			f=c; 
			itoa(f,buf,2);   //将f转换为二进制表示的字符串
			f=strlen(buf); 
			for(l=8;l>f;l--) 
			{
				strcat(header[i].bits,"0"); 
			}
			strcat(header[i].bits,buf); 
		} 
		header[i].bits[p]=0; 
	} 
	for(i=0;i<n;i++)   //根据哈夫曼编码的长短，对结点进行排序
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
	while(1)    //通过哈夫曼编码的长短，依次解码，从原来的位存储还原到字节存储
	{
		while(strlen(bx)<(unsigned int)p) 
		{
			fread(&c,1,1,ifp); 
			f=c; 
			itoa(f,buf,2); 
			f=strlen(buf); 
			for(l=8;l>f;l--) //在单字节内对相应位置补0
			{
				strcat(bx,"0"); 
			}
			strcat(bx,buf); 
		}
		for(i=0;i<n;i++) 
		{
			if(memcmp(header[i].bits,bx,header[i].count)==0) break; 
		}
		strcpy(bx,bx+header[i].count);   /*从压缩文件中的按位存储还原到按字节存储字符，
		字符位置不改变*/
		c=header[i].b; 
		fwrite(&c,1,1,ofp); 
		m++;   //统计解压缩后文件的长度
		if(m==flength) break;   //flength是原文件长度
	} 
	fclose(ifp); 
	fclose(ofp); 
	printf("\n\t解压缩文件成功!\n"); 
	if(m==flength)   //对解压缩后文件和原文件相同性比较进行判断（根据文件大小）
		printf("\t解压缩文件与原文件相同!\n\n"); 
	else printf("\t解压缩文件与原文件不同!\n\n");
	return; 
}
/*主函数*/
int main() 
{
	int c; 
	while(1)   //菜单工具栏
	{
		printf("\t _________________________________________________________\n");
		printf("\n");
		printf("\t         * 利用huffman编码对文本文件进行压缩与解压 *      \n");
		printf("\t _________________________________________________________\n");   
		printf("\t _________________________________________________________\n");
		printf("\t                               			                \n");   
		printf("\t 1.压缩                 			                        \n");   
		printf("\t 2.解压缩                                			    \n");   
		printf("\t 0.退出                                  		        \n");
		printf("\t__________________________________________________________\n");
	 
		printf("\n");
		do   //对用户输入进行容错处理
		{
			printf("\n\t*请选择相应功能(0-2):");     
			c=getch(); 
			printf("%c\n",c); 
			if(c!='0' && c!='1' && c!='2')
			{ 
				printf("\t@_@请检查您的输入在0~2之间！\n");
				printf("\t请再输入一遍！\n");
			}
		}while(c!='0' && c!='1' && c!='2'); 
		if(c=='1') compress();          //调用压缩子函数
		else if(c=='2') uncompress();   //调用解压缩子函数
		else 
		{
			printf("\t欢迎您再次使用该工具^_^\n"); 
			exit(0);                    //退出该工具
		}
		system("pause");   //任意键继续
		system("cls");     //清屏
	}
	return 0;
}
