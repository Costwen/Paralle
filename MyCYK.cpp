#pragma GCC optimize(1)
#pragma GCC optimize(2)
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC target("avx2,popcnt,lzcnt,abm,bmi,bmi2,fma,tune=native")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("-fgcse")
#pragma GCC optimize("-fgcse-lm")
#pragma GCC optimize("-fipa-sra")
#pragma GCC optimize("-ftree-pre")
#pragma GCC optimize("-ftree-vrp")
#pragma GCC optimize("-fpeephole2")
#pragma GCC optimize("-ffast-math")
#pragma GCC optimize("-fsched-spec")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("-falign-jumps")
#pragma GCC optimize("-falign-loops")
#pragma GCC optimize("-falign-labels")
#pragma GCC optimize("-fdevirtualize")
#pragma GCC optimize("-fcaller-saves")
#pragma GCC optimize("-fcrossjumping")
#pragma GCC optimize("-fthread-jumps")
#pragma GCC optimize("-funroll-loops")
#pragma GCC optimize("-freorder-blocks")
#pragma GCC optimize("-fschedule-insns")
#pragma GCC optimize("inline-functions")
#pragma GCC optimize("-ftree-tail-merge")
#pragma GCC optimize("-fschedule-insns2")
#pragma GCC optimize("-fstrict-aliasing")
#pragma GCC optimize("-falign-functions")
#pragma GCC optimize("-fcse-follow-jumps")
#pragma GCC optimize("-fsched-interblock")
#pragma GCC optimize("-fpartial-inlining")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("-freorder-functions")
#pragma GCC optimize("-findirect-inlining")
#pragma GCC optimize("-fhoist-adjacent-loads")
#pragma GCC optimize("-frerun-cse-after-loop")
#pragma GCC optimize("inline-small-functions")
#pragma GCC optimize("-finline-small-functions")
#pragma GCC optimize("-ftree-switch-conversion")
#pragma GCC optimize("-foptimize-sibling-calls")
#pragma GCC optimize("-fexpensive-optimizations")
#pragma GCC optimize("-fdelete-null-pointer-checks")
#pragma GCC optimize("Ofast", "inline", "-ffast-math")
#pragma GCC diagnostic error "-std=c++11"

#include <iostream>
#include <algorithm>
#include <omp.h>
#include <time.h>
#include <memory.h>
using namespace std;

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

struct BeginAndNum
{
    int begin=-1;
    unsigned num;
};

struct Production2
{
    int parent;
    int child1;
    int child2;
} production2[MAX_PRODUCTION2_NUM];

struct Production1
{
    int parent;
    char child;
} production1[MAX_PRODUCTION1_NUM];

BeginAndNum vnIndex[MAX_VN_NUM][MAX_VN_NUM];
BeginAndNum vtIndex[MAX_VT_NUM];

char str[MAX_STRING_LENGTH];

struct SubTree
{
    int root;
    unsigned num;
};
int subTreeTable[MAX_STRING_LENGTH][MAX_STRING_LENGTH][MAX_VN_NUM];
int TableSize[MAX_STRING_LENGTH][MAX_STRING_LENGTH][MAX_VN_NUM];
int vn_num;
int production2_num;
int production1_num;
int string_length;

int main()
{
    freopen("input.txt", "r", stdin);
    scanf("%d\n", &vn_num);
    scanf("%d\n", &production2_num);
    for (int i = 0; i < production2_num; i++)
        scanf("<%d>::=<%d><%d>\n", &production2[i].parent, &production2[i].child1, &production2[i].child2);
    scanf("%d\n", &production1_num);
    for (int i = 0; i < production1_num; i++)
        scanf("<%d>::=%c\n", &production1[i].parent, &production1[i].child);
    scanf("%d\n", &string_length);
    scanf("%s\n", str);

    sort(production1, production1 + production1_num, [](const Production1& a, const Production1& b)
    {
        return a.child == b.child ? a.parent < b.parent : a.child < b.child;
    });

    for (int i = 0; i < production1_num; i++)
    {
        int t = production1[i].child;
        if (vtIndex[t].begin == -1)
            vtIndex[t].begin = i;
        vtIndex[t].num++;
    }
    for (int i = 0; i < string_length; i++)
    {
        int t = str[i];
        int begin = vtIndex[t].begin;
        int end = begin + vtIndex[t].num;
        for (int j = begin; j < end; j++)
        {
            subTreeTable[i][i][production1[j].parent] = 1;
            TableSize[i][i][++TableSize[i][i][0]] = production1[j].parent;
        }
    }
    sort(production2, production2 + production2_num, [](const Production2& a, const Production2& b)
    {
        return a.child1 == b.child1 ?
            (a.child2 == b.child2 ? a.parent < b.parent : a.child2 < b.child2)
            : a.child1 < b.child1;
    });

    for (int i = 0; i < production2_num; i++)
    {
        int n1 = production2[i].child1;
        int n2 = production2[i].child2;
        if (vnIndex[n1][n2].begin == -1)
            vnIndex[n1][n2].begin = i;
        vnIndex[n1][n2].num++;
    }
    for (int len = 2; len <= string_length; len++)
    {
        #pragma omp parallel for
        for (int left = 0; left < string_length - len + 1; left++)
        {
            for (int right = left + 1; right < left + len; right++){
                for (int it1 = 1; it1 <= TableSize[left][right-1][0]; it1++)
                    for (int it2 = 1; it2 <= TableSize[right][left+len-1][0]; it2++){
                        // 枚举所有的产生式
                        int A = TableSize[left][right-1][it1];
                        int B = TableSize[right][left+len-1][it2];
                        if (vnIndex[A][B].num == 0)
                            continue;
                        int num1 = subTreeTable[left][right-1][A];
                        int num2 = subTreeTable[right][left+len-1][B];
                        int begin = vnIndex[A][B].begin; // 获取当前产生式的左边的范围
                        int end = begin + vnIndex[A][B].num; 

                        for (int i = begin; i < end; i++)
                        {
                            if (!subTreeTable[left][left+len-1][production2[i].parent])
                                TableSize[left][left+len-1][++TableSize[left][left+len-1][0]] = production2[i].parent;
                            subTreeTable[left][left+len-1][production2[i].parent] += num1 * num2;
                        }
                    }
            }
        }
    }

    printf("%u\n", subTreeTable[0][string_length - 1][0]);
    return 0;
}