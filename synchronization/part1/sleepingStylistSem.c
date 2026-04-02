#define DELAY 100000 // adjust this value

void main(void) {
  // create 75 of customer threads
  // Create one stylist thread. Don't forget to join threads
}

void stylist (void) {
  // add more variables as needed
  int j;
  while (1) {
    mon_debugPrint();
    mon_checkCustomer();
    for (j=0; j < DELAY; j++); // Cut hair
  }
}

void customer(void) {
  // add more variables as needed
  int j;
  while (1) {
    mon_debugPrint();
    if (mon_checkStylist()) break;
    for (j=0; j<DELAY; j++); // Go shopping
  }
}