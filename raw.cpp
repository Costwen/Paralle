#pragma GCC optimize(1)
#pragma GCC optimize(2)
#pragma GCC optimize(3)
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
#pragma GCC optimize("inline-functions-called-once")
#pragma GCC optimize("-fdelete-null-pointer-checks")

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
    int begin = -1;
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
} subTreeTable[MAX_STRING_LENGTH][MAX_STRING_LENGTH][MAX_VN_NUM];

int subTreeNumTable[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

SubTree subTreeBuf[2][MAX_STRING_LENGTH];
SubTree subTreeChild1;
SubTree subTreeChild2;
SubTree subTreeParent;
int begin, end;

int vn_num;
int production2_num;
int production1_num;
int string_length;

inline void loop6(int j, int &k, int oldTreeNum, int &newTreeNum, int last, int curr)
{
    subTreeParent.root = production2[j].parent;
    subTreeParent.num = subTreeChild1.num * subTreeChild2.num;
    
    while (k < oldTreeNum && subTreeParent.root > subTreeBuf[last][k].root)
        subTreeBuf[curr][newTreeNum++] = subTreeBuf[last][k++];

    if (k < oldTreeNum && subTreeParent.root == subTreeBuf[last][k].root)
        subTreeParent.num += subTreeBuf[last][k++].num;
    subTreeBuf[curr][newTreeNum++] = subTreeParent;
}

inline void loop5(int i1, int i2, int &last, int &curr, int len, int left, int right, int &oldTreeNum)
{
    subTreeChild2 = subTreeTable[right][left + len - 1][i2];
    int begin = vnIndex[subTreeChild1.root][subTreeChild2.root].begin;
    int end = begin + vnIndex[subTreeChild1.root][subTreeChild2.root].num;
    if (begin == end)
    {
        return;
    }
    swap(last, curr);
    int newTreeNum = 0;
    int k = 0;
    for (int j = begin; j < end; j++)
    {
        loop6(j, k, oldTreeNum, newTreeNum, last, curr);
    }

    while (k < oldTreeNum)
    {
        subTreeBuf[curr][newTreeNum++] = subTreeBuf[last][k++];
        
    }
    oldTreeNum = newTreeNum;
}

inline void loop4(int len, int left, int right, int i1, int &last, int &curr, int &oldTreeNum)
{
    subTreeChild1 = subTreeTable[left][right - 1][i1];
    for (int i2 = 0; i2 < subTreeNumTable[right][left + len - 1]; i2++)
    {
        loop5(i1, i2, last, curr, len, left, right, oldTreeNum);
    }
}

inline void loop3(int len, int left, int right, int &last, int &curr, int &oldTreeNum)
{
    for (int i1 = 0; i1 < subTreeNumTable[left][right - 1]; i1++)
    {
        loop4(len, left, right, i1, last, curr, oldTreeNum);
    }
}
inline void loop2(int len, int left, int &last, int &curr, int &oldTreeNum)
{
    for (int right = left + 1; right < left + len; right++)
    {
        loop3(len, left, right, last, curr, oldTreeNum);
    }
}
inline void loop1(int len)
{
    for (int left = 0; left <= string_length - len; left++)
    {
        int curr = 0;
        int last = 1;
        int oldTreeNum = 0;
        loop2(len, left, last, curr, oldTreeNum);
        subTreeNumTable[left][left + len - 1] = oldTreeNum;
        if (subTreeNumTable[left][left + len - 1] > 0)
        {
            memcpy(subTreeTable[left][left + len - 1], subTreeBuf[curr], subTreeNumTable[left][left + len - 1] * sizeof(SubTree));
        }
    }
}
int main(int argc, int **argv)
{
    double start_time, end_time, init_time;
    init_time = start_time = clock();
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
    end_time = clock();

    printf("Input Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();
    sort(production1, production1 + production1_num, [](const Production1 &a, const Production1 &b)
         { return a.child == b.child ? a.parent < b.parent : a.child < b.child; });
    end_time = clock();

    printf("Sort Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();

    for (int i = 0; i < production1_num; i++)
    {
        int t = production1[i].child;
        if (vtIndex[t].begin == -1)
            vtIndex[t].begin = i;
        ++vtIndex[t].num;
    }
    end_time = clock();

    printf("Production1 Init Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();
    int begin, end, t;
    for (int i = 0; i < string_length; i++)
    {
        t = str[i];
        begin = vtIndex[t].begin;
        end = begin + vtIndex[t].num;
        for (int j = begin; j < end; j++)
        {
            subTreeTable[i][i][subTreeNumTable[i][i]].root = production1[j].parent;
            subTreeTable[i][i][subTreeNumTable[i][i]].num = 1;
            ++subTreeNumTable[i][i];
        }
    }

    end_time = clock();

    printf("Production2 Init Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();

    sort(production2, production2 + production2_num, [](const Production2 &a, const Production2 &b)
         { return a.child1 == b.child1 ? (a.child2 == b.child2 ? a.parent < b.parent : a.child2 < b.child2)
                                       : a.child1 < b.child1; });
    end_time = clock();

    printf("Sort Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();
    end_time = clock();
    printf("Init Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();

    for (int i = 0; i < production2_num; i++)
    {
        int n1 = production2[i].child1;
        int n2 = production2[i].child2;
        if (vnIndex[n1][n2].begin == -1)
            vnIndex[n1][n2].begin = i;
        vnIndex[n1][n2].num++;
    }
    end_time = clock();
    printf("Count Init Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);
    start_time = clock();
    int len, left, right, i1, i2, j, k, newTreeNum;
    int curr, last, oldTreeNum, copy_len;
    for (len = 2; len <= string_length; len++)
    {
        loop1(len);
    }
    end_time = clock();
    printf("Production2 Time: %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);
    unsigned treeNum = 0;
    if (subTreeNumTable[0][string_length - 1] > 0)
    {
        if (subTreeTable[0][string_length - 1][0].root == 0)
        {
            treeNum = subTreeTable[0][string_length - 1][0].num;
        }
    }
    printf("%u\n", treeNum);
    end_time = clock();
    printf("Total Time: %.3lf\n", (end_time - init_time) / CLOCKS_PER_SEC);
    return 0;
}