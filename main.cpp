#include <iostream>
#include <ctime>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <ostream>

using namespace std;

const int MaxIter=100000;
void string2arc(const string &a,vector<int> &b,const string &delim){
    string::size_type pos1=a.find(delim),pos2=a.find(delim,pos1+1);
    string str1=a.substr(pos1,pos2-pos1),str2=a.substr(pos2);
    const char *num_str1=str1.c_str(),*num_str2=str2.c_str();
    b[0]=atoi(num_str1);
    b[1]=atoi(num_str2);

}
void string2vex(const string &a,vector<int> &b,const string &delim){
    string::size_type pos0=a.find(delim),pos1=a.find(delim,pos0+1)+1,pos2=a.find(delim,pos1+1);
    string str1=a.substr(pos1,pos2-pos1);
    string str2=a.substr(pos2);

    const char *num_str1=str1.c_str(),*num_str2=str2.c_str();
    b.push_back(atoi(num_str1));
    b.push_back(atoi(num_str2));
    cout<<atoi(num_str1)<<" "<<atoi(num_str2)<<endl;

}

class tabu_search{
public:
    int *Sol;//节点对应颜色值
    int f;//冲突值
    int f_table[2][1000];//冲突维护表
  //  int *BestSol;//最优解
    int best_f;//历史最优冲突值
    long long **TabuTenure;//禁忌表
    int **Adjacent_Color_Table;//邻接颜色表
    int N,e;//点数，边数
    int K;//颜色数

   // int delt=10000;

    int **graph;//图--记录邻接边,方便更新邻接颜色表
    long long iter;//步数记录
    int tabu_best_move[2];//记录当前禁忌表中BEST_MOVE
    int non_tabu_best_move[2];//记录当前非禁忌表中BEST_MOVE

    void put_color_num(int color_num){
        K=color_num;
    }
//初始化各数据结构
    void Initialization(){
        ifstream infile("DSJC500.5.col.txt",ios::in);
        string delim=" ";
        string str;
        if(!infile.good())
        {
            cout<<"file open error"<<endl;
            return;
        }
        vector<int> num;

        f=0;
        for(int i=0;i<1000;i++)
        {
            f_table[0][i]=0;
            f_table[1][i]=0;
        }
        while(!infile.fail()){
            getline(infile,str);

            if(str.find("p")==0){

                string2vex(str,num,delim);
                N=num[0];
                e=num[1];

                //内存分配
                Sol=new int[N];
                Adjacent_Color_Table=new int *[N];
                graph=new int*[N];
                TabuTenure=new long long*[N];


                //数值初始化
                for(int i=0;i<N+1;i++){
                    Adjacent_Color_Table[i]=new int[K];
                    TabuTenure[i]=new long long[K];
                    graph[i]=new int[N];

                    for(int j=0;j<K;j++) {
                        Adjacent_Color_Table[i][j] = 0;
                        TabuTenure[i][j] = 0;
                    }

                    graph[i][0]=0;
                    for(int j=1;j<N;j++)
                        graph[i][j]=-1;

                }

                for(int i=0;i<N;i++){
                    Sol[i]=rand()%K;//初始解
                }
            }
            else if(str.find("e")==0){
                string2arc(str,num,delim);
                int first=num[0]-1,next=num[1]-1;//对应数组下标

                //邻接颜色表统计+图数据初始化
                graph[first][0]++;
                graph[next][0]++;
                graph[first][graph[first][0]]=next;
                graph[next][graph[next][0]]=first;

                Adjacent_Color_Table[first][Sol[next]]++;
                Adjacent_Color_Table[next][Sol[first]]++;

                if(Sol[first]==Sol[next]){
                    //冲突数记录
                    f++;
                    if(!f_table[1][first]){
                        f_table[0][0]++;
                        f_table[0][f_table[0][0]]=first;
                        f_table[1][first]=f_table[0][0];
                    }
                    if(!f_table[1][next]){
                        f_table[0][0]++;
                        f_table[0][f_table[0][0]]=next;
                        f_table[1][next]=f_table[0][0];
                    }
                }
            }

        }
        best_f=f;
        infile.close();
    }

   void FindMove(int &u,int &vi,int &vj,int &delt){
        //void FindMove(){
        delt=10000;
        int delt1=10000;
        int count=0,tabu_count=0;
       int equ_delt[2000][2];//非禁忌相同delt值
       int equ_tabudelt[2000][2];//禁忌相同delt值

//        //STUPID ME
//        for(int i=0;i<N;i++){//全部查找一遍
//        //在冲突表中查找
//        if(!Adjacent_Color_Table[i][Sol[i]]) continue;
        int i;
        for(int m=1;m<=f_table[0][0];m++){
            i=f_table[0][m];//冲突节点赋值

              //  int count=1;//记录delt相同情况

                for(int k=0;k<K;k++){//遍历颜色搜索冲突min

                    if(k==Sol[i]) continue;

                    int tmp_delt=Adjacent_Color_Table[i][k]-Adjacent_Color_Table[i][Sol[i]];//计算DELT

                    if(tmp_delt>delt)
                        continue;

                    //非禁忌移动
                    if(TabuTenure[i][k]<iter) {
                            if (tmp_delt< delt) {
                                count = 0;
                                delt = tmp_delt;
                            }
                            count++;
                            equ_delt[count - 1][0] = i;
                            equ_delt[count - 1][1] = k;


                        }
                     //禁忌移动
                        else{
                             if(tmp_delt<=delt1){
                                 if(tmp_delt<delt1){
                                     delt1=tmp_delt;
                                     tabu_count=0;
                                 }
                                 tabu_count++;
                                 equ_tabudelt[tabu_count - 1][0] = i;
                                 equ_tabudelt[tabu_count - 1][1] = k;
                             }

                        }
                }//结束当前节点的颜色判断
        }//结束统计比较

        //最终判断
        int tmp=0;
        if(f+delt1<best_f)
        {
            delt=delt1;
            tmp = rand() % tabu_count;//相等delt随机选择
            u= equ_tabudelt[tmp][0];
            vj= equ_tabudelt[tmp][1];
//            u=tabu_best_move[0];
//            vj=tabu_best_move[1];
        }
        else{
            tmp = rand() % count;//相等delt随机选择
            u= equ_delt[tmp][0];
            vj= equ_delt[tmp][1];
//            u=non_tabu_best_move[0];
//            vj=non_tabu_best_move[1];
        }
      vi=Sol[u];
    }
    void MakeMove(int &u,int &vi,int &vj, int &delt){
    //void MakeMove(){

        Sol[u]=vj;
        f=f+delt;
        if(best_f>f)
            best_f=f;
        //更新禁忌表
        TabuTenure[u][vi]=iter+f+rand()%10;

        int tmp;
        int i;
        int pos,final,count;
        //邻接颜色表+冲突表更新
        for(int m=1;m<=graph[u][0];m++){
                i=graph[u][m];
                tmp=Sol[i];
                Adjacent_Color_Table[i][vi]--;
                Adjacent_Color_Table[i][vj]++;

                if(vi==tmp&&!Adjacent_Color_Table[i][tmp]){//若修改颜色后冲突消除,在冲突表中删除
                    pos=f_table[1][i],count=f_table[0][0],final=f_table[0][count];
                    f_table[0][pos]=f_table[0][count];//最后一个往前丢
                    f_table[1][final]=pos;//修改最后一个节点位置
                    f_table[1][i]=0;//位置清空
                    f_table[0][0]--;//数量减少
                }
                else if(vj==tmp&&!f_table[1][i]){//修改颜色后产生新冲突
                    f_table[0][0]++;
                    pos=f_table[0][0];
                    f_table[0][pos]=i;//末尾插入
                    f_table[1][i]=pos;//记录位置
                   //cout<<i<<endl;
                }
        }

        if(f_table[1][u]&&!Adjacent_Color_Table[u][Sol[u]]){//变色后若冲突消除
            pos=f_table[1][u],count=f_table[0][0],final=f_table[0][count];
            f_table[0][pos]=f_table[0][count];
            f_table[1][final]=pos;//修改最后一个节点位置
            f_table[1][u]=0;
            f_table[0][0]--;
        }
    }
    void run(){
        iter=0;
        ofstream ofile("/Users/jokerpwn/Desktop/instances/500.5test_seventh.txt ", ios::out);
        if(!ofile.good())
        {
            cout<<"file open error"<<endl;
            return;
        }
        srand(clock());
        ofile<<"随机种子"<<clock()<<endl;
        Initialization();

        double  start_time,end_time,elapsed_time;
        start_time=clock();

        int u,vi,vj,delt;
        while(f>0){//冲突仍存在
            iter++;
            FindMove(u,vi,vj,delt);
            MakeMove(u,vi,vj,delt);
           if(iter%100000==0)
                cout<<best_f<<" "<<delt<<" "<<iter<<" "<<f<<endl;
        }
        end_time=clock();
        elapsed_time=(end_time - start_time)/CLOCKS_PER_SEC;

        ofile<<"颜色数"<<K<<"解具体情况："<<endl;
        for(int i=0;i<N;i++)
            ofile<<i+1<<" "<<Sol[i]<<endl;
        ofile<<"时间"<<elapsed_time<<endl;
        ofile<<"迭代次数"<<iter<<endl;

       ofile.close();
    }

};
int main(){
    tabu_search init;
    int color_num;
    cout<<"输入颜色数:";
    cin>>color_num;
    init.put_color_num(color_num);

    init.run();
}