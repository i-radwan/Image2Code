#include<iostream>
#include <set>
using namespace std;
int main(){
int n, m, h, id, day;
set<int> ans;
cin>>n>>m>>h;
for (int i=0;i<h;++i) {
cin>> id>>day;
if (day > n-m) {
 ans.insert (id);
}
}
cout<<ans.size();
return 0;
}
