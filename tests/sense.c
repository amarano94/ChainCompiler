//chain sense example, in C to use a chain plugin


origin task Sense(){
   int s = sensor;
   ChOut {S <- s}, CmpArg;
   NextTask CmpAvg;
}

task CmpAvg(){
   int s = ChIn S, Sense;
   int head = ChIn HEAD, self;

   int sum=0, avg=0;
   for (int i=0; i < 5; i++){
      sum += ChIn a[i], self;
   }
   avg = sum / 5;

   ChOut {a[head] <- s}, self;
   head = (head+1)%5;
   ChOut {HEAD <- head}, self;

   if( s> avg*2){
     ChOut {S <- s}, Alert;
     NextTask Alert;
   }

   NextTask Sense;
}

task Alert(){
   int s = ChIn S, CmpAvg;
   int cnt = ChIn Cnt, self;

   /* external func to report
      anomaly val & count */
   report(s, cnt);

   ChOut {Cnt <- cnt+1}, self;
   NextTask Sense;
}
