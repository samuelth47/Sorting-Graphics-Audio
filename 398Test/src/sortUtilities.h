/* sortUtilities.h
 *
 */

#include <vector>
using namespace std;

#define MAX_VALUE 601
#define GRAPHIC_WAIT 0.0000001
#define SIZE 2500

bool sorted(const vector<int>& data) {
   for (unsigned i = 1; i < data.size(); ++i) {
      if (data[i-1] > data[i]) {
         return false;
      }
   }
   return true;
}

void initialize(vector<int>& v) {
  srand(1876);
  for (unsigned i = 0; i < v.size(); ++i) {
    v[i] = rand() % MAX_VALUE;
  }
}

