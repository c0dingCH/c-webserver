#include <iostream>
#include <unistd.h>
#include "AsyncLogging.h"
using namespace std;

int main(){
  AsyncLogging as("../tmp.log");
  as.Start();
  as.Append("123456789\n", 10);
  
  sleep(10);

  as.Append("123456789", 9);
 
  as.Stop();
}
