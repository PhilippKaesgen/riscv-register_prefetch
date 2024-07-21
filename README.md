# Example

```
#include "warm_up.h"
#define SIZE 1024
unsigned input[] = { [0 ... (SIZE-1)] = 10};
char var = 6;
int main() {
  warm_up_rd(input, sizeof(input))
  warm_up_rd(&var, sizeof(var))
}
```
