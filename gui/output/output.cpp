#include <stdio.h>
#include <iostream>
using namespace std;
int a, b, c, d, mx, md, mn;
int main() {
while (cin >> a >> b >> c >> d) {

mx = 2 * a;

md = 2 * b;

mn = c < d ? c : d;

mn *= 2;

if (d > mn || c > mn || 2*d >= mx || 2*d >= md) {
cout << "-1" << endl;
}
else {
cout << mx << endl << md << endl << mn << endl;
}
}

return 0;

}