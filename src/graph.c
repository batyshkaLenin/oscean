// You cannot be that which you observe

void fputs_graph_burn(FILE *f, Journal *journal) {
  int segs[52];

  // The Arvelie calendar has 26 months of 14 days each.
  // Blank out array
  for (int i = 0; i < 52; ++i) {
    segs[i] = 0;
  }

  // Populate last logs with offset
  for (int i = 0; i < 52 * 14; ++i) {
    Log l = journal->logs[i];
    int offset = offset_from_arvelie(l.date);
    if (offset < 0) {
      continue;
    }
    int seg = (offset / 14);
    if (seg >= 52) {
      continue;
    }
    segs[seg] += l.code % 10;
  }

  float average = find_average(segs);

  // Calculate Offset
  float offsets[52];
  for (int i = 0; i < 52; ++i) {
    offsets[i] = (segs[51 - i] - average) * -2;
  }

  // Bleed
  float bleed[52];
  float prev = offsets[0];
  float prev2 = offsets[0];
  for (int i = 0; i < 52; ++i) {
    prev2 = prev;
    bleed[i] = (offsets[i] + prev + prev2) / 3;
    prev = bleed[i];
  }

  // Draw
  int s = 11;
  int w = (s + 1) * 52;

  fprintf(f, "<figure>");
  fprintf(f, "<svg width='%d' height='%d' xmlns='http://www.w3.org/2000/svg' class='burn'>", w, (s+1) * 7);

  // Middle
  fprintf(f, "<line x1='0' y1='42' x2='700' y2='42' style='fill:none;stroke:black;stroke-width:1;stroke-linejoin:round;stroke-dasharray:0,2; stroke-linecap:round'/>");

  // Phase
  int phase = 0;
  int last_phase_pos = 0;
  
  // Path
  last_phase_pos = 0;
  fprintf(f, "<path d='M0,42 ");
  for (int i = 0; i < 52; ++i) {
    if(i == 0){ continue; }
    if(bleed[i-1] > 0 && bleed[i] > 0){ continue; }
    if(bleed[i-1] < 0 && bleed[i] < 0){ continue; }
    if(bleed[i-1] < 0 && bleed[i] > 0){ phase = 1; }
    if(bleed[i-1] > 0 && bleed[i] < 0){ phase = -1; }
    // phase shift
    int x = i * (s+1);
    fprintf(f, "L%d,%d ", last_phase_pos, (-20 * phase) + 42); 
    fprintf(f, "L%d,%d ", x, (-20 * phase) + 42); 
    last_phase_pos = x;
  }
  fprintf(f, "L%d,%d ", last_phase_pos, (-20 * -phase) + 42); 
  fprintf(f, "L%d,%d ", w, (-20 * -phase) + 42); 
  fprintf(f, "' style='fill:none;stroke:#ddd;stroke-width:1;stroke-linejoin:round'/>");

  // bleed
  fprintf(f, "<path d='");
  for (int i = 0; i < 52; ++i) {
    int x = i * (s+1) + (s/2);
    int y = clamp_int(bleed[i], -36, 36) + 42;
    fprintf(f, "M%d,%d L%d,%d ", x, 42, x, y);    
  }
  fprintf(f, "' style='fill:none;stroke:black;stroke-width:12'/>");

  for (int i = 0; i < 52; ++i) {
    if(i == 0){ continue; }
    if(bleed[i-1] > 0 && bleed[i] > 0){ continue; }
    if(bleed[i-1] < 0 && bleed[i] < 0){ continue; }
    if(bleed[i-1] < 0 && bleed[i] > 0){ phase = 1; }
    if(bleed[i-1] > 0 && bleed[i] < 0){ phase = -1; }
    // phase shift
    int x = i * (s+1);
    if(phase == 1){
      fprintf(f, "<polyline points='%d %d %d %d %d %d' style='fill:black;stroke:none'/>", x, 84-5, x+5, 84, x-5, 84);    
    }
    else if(phase == -1){
      fprintf(f, "<polyline points='%d %d %d %d %d %d' style='fill:black;stroke:none'/>", x, 5, x+5, 0, x-5, 0);  
    }
    last_phase_pos = x;
  }

  fprintf(f, "</svg>");
  fprintf(f, "<figcaption>Fig. Cycles of burn and rest of previous %d days.</figcaption>", 52 * 14);
  fprintf(f, "</figure>");
}