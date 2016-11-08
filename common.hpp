extern "C" {

  extern struct {
    int idnevent;
    float obs[13];
  } pawidn_;

  extern struct {
    bool chain;
    int nchevt;
    int ichevt;
  } pawchn_;

  extern struct {
    char cfile[128];
  } pawch_;


  extern struct {
    int ievent;
    int itime;
    int iatime;
    int n1;
    int n2;
    int n3;
    int n4;
    int n5;
    int n6;
    int n7;
    int n8;
    int n9;
    int n10;
    int n11;
    int ivfas_strip1[390];
    int ivfas_strip2[390];
    int ivfas_strip3[390];
    int ivfas_strip4[390];
    int ivfas_strip5[390];
    int ivfas_strip6[390];
    int ivfas_strip7[390];
    int ivfas_strip8[390];
    int ivfas_strip9[390];
    int ivfas_strip10[390];
    int ivfas_strip11[390];
    int ivfas_data1[390];
    int ivfas_data2[390];
    int ivfas_data3[390];
    int ivfas_data4[390];
    int ivfas_data5[390];
    int ivfas_data6[390];
    int ivfas_data7[390];
    int ivfas_data8[390];
    int ivfas_data9[390];
    int ivfas_data10[390];
    int ivfas_data11[390];
    float xinfo[10];
    int xinfo_plus[10];
  } pawcr4_;

}
