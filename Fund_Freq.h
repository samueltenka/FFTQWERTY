//naive algorithm for now.

#ifndef FUND_FREQ_H
#define FUND_FREQ_H

#include <vector>
#include <queue>

/*e.g.:
float a[8] = {3.0, 2.0, 4.5, 1.0, 5.0, 2.1, 4.0, 4.1};
fund_freq(8, a, 2) == 4.5 // first element among top 2 elements of this length-8 array*/

std::vector<int> peaks(int len, float a[], int n) {
   /*modified from http://stackoverflow.com/questions/14902876/indices-of-the-k-largest-elements-in-an-unsorted-length-n-array*/
   std::priority_queue<std::pair<float,int>> qq;
   for(int i=0; i<n; ++i) {qq.push({0.0, 0});}
   for (int i = 0; i<len; ++i) {
      if(-a[i] >= qq.top().first) {continue;}
      qq.push(std::pair<float, int>(-a[i], i));
      qq.pop();
   }
   std::vector<int> rtrn;
   while(!qq.empty()) {
      rtrn.push_back(qq.top().second);
      qq.pop();
   }
   return rtrn;
}

int fund_freq(int len, float a[], int n) {
   std::vector<int> v = peaks(len, a, n);
   int min=len;
   for(std::vector<int>::const_iterator i=v.begin(); i!=v.end(); ++i) {
      if(*i>min) {continue;}
      min=*i;
   }
   return min;
}

#endif FUND_FREQ_H
