// A parameter is a value that can be easily incremented/decremented without
// overflowing.
struct Parameter {
  int value;
  int max; // maximum value, inclusive
  bool wraparound;

  Parameter(int initial, int max, bool wraparound=false)
    : value(initial), max(max), wraparound(wraparound) {
  }

  void increment() {
    if (value < max) {
      value++;
    } else if (wraparound) {
      value = 0;
    }
  }
  void decrement() {
    if (value > 0) {
      value--;
    } else if (wraparound) {
      value = max;
    }
  }
};
