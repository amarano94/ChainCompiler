//chain sense example, in C to use a chain plugin

//task Sense(){
   //int s = sensor;
   //ChOut {S <- s}, CmpArg;
   //NextTask CmpAvg;
//}
//

int main() {
    int s = 0;
    int hello = ChIn s;
    ChOut a;
    NextTask CmpAvg;
    return s;
}

