//#include <rochcor.h>
#include "FinalStateAnalysis/PatTools/interface/RochesterCorrections2011.h"
#include <TLorentzVector.h>

#include <CLHEP/Random/RandGaussQ.h>
#include <CLHEP/Random/Random.h>

#include "FWCore/Utilities/interface/Exception.h"

#define rochcor_f const float RochesterCorrections2011::
#define rochcor_d const double RochesterCorrections2011::


namespace rochcor {

  rochcor_d pi = 3.14159265358979323846;
  rochcor_f genm_smr = 9.09956e+01; //gen mass peak with eta dependent gaussian smearing => better match in Z mass profile vs. eta/phi
  rochcor_f genm = 91.06; //gen mass peak without smearing => Z mass profile vs. eta/phi in CMS note

  rochcor_f recmA = 9.10062e+01; //rec mass peak in MC (2011A)
  rochcor_f drecmA = 9.09285e+01; //rec mass peak in data (2011A)
  rochcor_f mgsclA_stat = 0.0001; //stat. error of global factor for mass peak in MC (2011A)
  rochcor_f mgsclA_syst = 0.0006; //syst. error of global factor for mass peak in MC (2011A)
  rochcor_f dgsclA_stat = 0.0001; //stat. error of global factor for mass peak in data (2011A)
  rochcor_f dgsclA_syst = 0.0008; //syst. error of global factor for mass peak in data (2011A)
  rochcor_f recmB = 9.10210e+01; //rec mass peak in MC (2011B)
  rochcor_f drecmB = 9.09469e+01; //rec mass peak in data (2011B)
  rochcor_f mgsclB_stat = 0.0001; //stat. error of global factor for mass peak in MC (2011B)
  rochcor_f mgsclB_syst = 0.0006; //syst. error of global factor for mass peak in MC (2011B)
  rochcor_f dgsclB_stat = 0.0001; //stat. error of global factor for mass peak in data (2011B)
  rochcor_f dgsclB_syst = 0.0008; //syst. error of global factor for mass peak in data (2011B)

  //iteration2 after FSR : after Z Pt correction
  rochcor_f deltaA = -2.85242e-06;
  rochcor_f deltaA_stat = 7.74389e-07;
  rochcor_f deltaA_syst = 6.992e-07;

  rochcor_f sfA = 44.6463;
  rochcor_f sfA_stat = 1.92224;
  rochcor_f sfA_syst = 9.29;

  rochcor_f deltaB = -5.68463e-06;
  rochcor_f deltaB_stat = 8.21406e-07;
  rochcor_f deltaB_syst = 1.4268e-06;

  rochcor_f sfB = 23.8652;
  rochcor_f sfB_stat = 0.941748;
  rochcor_f sfB_syst = 4.86;

  rochcor_f apar = 1.0; //+- 0.002
  rochcor_f bpar = -5.03313e-06; //+- 1.57968e-06
  rochcor_f cpar = -4.41463e-05; //+- 1.92775e-06
  rochcor_f d0par = -0.000148871; //+- 3.16301e-06
  rochcor_f e0par = 1.59501; //+- 0.0249021
  rochcor_f d1par = 7.95495e-05; //+- 1.12386e-05
  rochcor_f e1par = -0.364823; //+- 0.17896
  rochcor_f d2par = 0.000152032; //+- 5.68386e-06
  rochcor_f e2par = 0.410195; //+- 0.0431732

  rochcor_f netabin[9] = {-2.4,-2.1,-1.4,-0.7,0.0,0.7,1.4,2.1,2.4};


  rochcor_f dcor_bfA[8][8]={{-0.000089062,-0.000042749,-0.000014325,-0.000025780,-0.000007783,-0.000011832,-0.000078398,-0.000027247},
			    {0.000020549,0.000008370,-0.000000723,-0.000012606,-0.000011076,-0.000010040,-0.000009078,0.000079329},
			    {0.000078765,-0.000039693,0.000004824,0.000011344,-0.000010456,-0.000002849,0.000006552,-0.000001531},
			    {-0.000039977,-0.000005774,0.000001251,-0.000038683,-0.000020391,-0.000028091,-0.000058890,-0.000050209},
			    {-0.000041154,-0.000005251,0.000012193,0.000006557,0.000008643,-0.000042624,-0.000066871,-0.000012433},
			    {-0.000061152,-0.000111091,-0.000042864,-0.000004620,-0.000008327,-0.000019330,-0.000083283,-0.000065904},
			    {-0.000097188,-0.000096252,-0.000026895,-0.000022658,-0.000015016,-0.000044672,-0.000071962,-0.000043877},
			    {-0.000095436,-0.000101665,-0.000023646,-0.000009728,-0.000017671,-0.000020580,-0.000062692,-0.000051380}};
  rochcor_f dcor_maA[8][8]={{0.000773765,0.000161235,-0.000170623,-0.000176999,-0.000102851,-0.000072180,0.000385588,0.001452750},
			    {0.001234179,0.000264309,-0.000111439,-0.000093614,-0.000078087,-0.000019007,0.000177556,0.000471992},
			    {0.000275687,0.000099339,0.000044930,0.000102909,0.000065894,0.000065247,0.000129964,0.000428338},
			    {-0.001579112,-0.000348688,0.000061190,0.000119775,0.000142235,0.000125262,0.000025557,0.000117348},
			    {-0.002078640,-0.000420932,0.000015967,0.000066302,0.000151720,0.000262669,0.000127636,-0.000478173},
			    {-0.001451419,-0.000409489,-0.000066307,-0.000009796,0.000028829,0.000044267,0.000055514,-0.000321381},
			    {0.000043991,-0.000101877,-0.000072701,-0.000048250,-0.000069956,-0.000101948,0.000001071,0.001258225},
			    {0.000196030,0.000027346,-0.000142681,-0.000131590,-0.000166195,-0.000192748,0.000129522,0.001698982}};
  rochcor_f mcor_bfA[8][8]={{-0.000085255,-0.000048429,-0.000008083,0.000019225,0.000005942,0.000002527,-0.000065219,-0.000094914},
			    {-0.000085427,-0.000031099,0.000002547,-0.000015030,-0.000018668,-0.000009178,-0.000016354,-0.000049979},
			    {-0.000047587,-0.000051665,-0.000010309,-0.000014792,-0.000009519,-0.000019372,-0.000035122,-0.000082948},
			    {-0.000073551,-0.000046823,0.000003655,-0.000000739,-0.000004009,-0.000002856,-0.000032493,-0.000100393},
			    {-0.000053099,-0.000034247,0.000002106,0.000011192,0.000001536,-0.000005138,-0.000033142,-0.000069466},
			    {-0.000044449,-0.000048159,0.000000505,0.000014757,0.000021576,-0.000007426,-0.000036307,-0.000083206},
			    {-0.000103425,-0.000063496,-0.000002600,0.000024105,0.000025119,-0.000004809,-0.000054657,-0.000070125},
			    {-0.000083552,-0.000044236,-0.000008486,0.000003650,-0.000000765,0.000004032,-0.000028714,-0.000085460}};
  rochcor_f mcor_maA[8][8]={{0.000931554,0.000110286,-0.000173071,-0.000181045,-0.000173514,-0.000193640,0.000025857,0.000149956},
			    {0.001019641,0.000002822,-0.000112803,-0.000107498,-0.000097084,-0.000057863,0.000192509,0.000776879},
			    {0.000501675,-0.000034478,0.000022787,0.000026778,0.000023658,0.000057978,0.000065871,0.000019960},
			    {0.000678294,0.000406294,0.000173642,0.000137412,0.000148881,0.000165471,0.000331316,0.000326927},
			    {0.000842580,0.000494548,0.000201404,0.000158079,0.000164143,0.000159690,0.000328212,0.000975395},
			    {-0.001170926,-0.000140921,0.000114452,0.000107551,0.000108058,0.000069775,0.000138121,0.000781180},
			    {-0.001714847,-0.000305253,-0.000029425,-0.000014065,-0.000024933,-0.000048147,-0.000138109,-0.000235413},
			    {0.000410902,0.000100405,-0.000135010,-0.000119392,-0.000119765,-0.000154755,-0.000784318,-0.002153571}};
  
  rochcor_f dcor_bfAer[8][8]={{0.000070789,0.000035670,0.000030378,0.000028489,0.000028671,0.000030529,0.000036211,0.000067427},
			      {0.000064513,0.000035484,0.000030192,0.000028476,0.000028440,0.000030524,0.000035467,0.000064404},
			      {0.000067093,0.000036572,0.000030410,0.000028231,0.000028303,0.000030111,0.000035645,0.000064843},
			      {0.000065660,0.000035923,0.000029991,0.000028861,0.000028512,0.000030372,0.000035161,0.000064553},
			      {0.000066659,0.000035781,0.000030601,0.000028248,0.000028450,0.000030543,0.000036124,0.000069993},
			      {0.000063792,0.000035191,0.000029819,0.000029165,0.000028749,0.000030072,0.000035116,0.000064668},
			      {0.000067209,0.000036002,0.000030016,0.000028625,0.000028902,0.000030066,0.000035301,0.000065695},
			      {0.000065983,0.000036060,0.000030378,0.000028378,0.000028530,0.000030026,0.000035840,0.000064573}};
  rochcor_f dcor_maAer[8][8]={{0.000070789,0.000035670,0.000030378,0.000028489,0.000028671,0.000030529,0.000036211,0.000067427},
			      {0.000064513,0.000035484,0.000030192,0.000028476,0.000028440,0.000030524,0.000035467,0.000064404},
			      {0.000067093,0.000036572,0.000030410,0.000028231,0.000028303,0.000030111,0.000035645,0.000064843},
			      {0.000065660,0.000035923,0.000029991,0.000028861,0.000028512,0.000030372,0.000035161,0.000064553},
			      {0.000066659,0.000035781,0.000030601,0.000028248,0.000028450,0.000030543,0.000036124,0.000069993},
			      {0.000063792,0.000035191,0.000029819,0.000029165,0.000028749,0.000030072,0.000035116,0.000064668},
			      {0.000067209,0.000036002,0.000030016,0.000028625,0.000028902,0.000030066,0.000035301,0.000065695},
			      {0.000065983,0.000036060,0.000030378,0.000028378,0.000028530,0.000030026,0.000035840,0.000064573}};
  rochcor_f mcor_bfAer[8][8]={{0.000028957,0.000015643,0.000013419,0.000012634,0.000012700,0.000013592,0.000016042,0.000028597},
			      {0.000027958,0.000015560,0.000013545,0.000012820,0.000012798,0.000013524,0.000015725,0.000027844},
			      {0.000027910,0.000015737,0.000013522,0.000012785,0.000012761,0.000013554,0.000015626,0.000027776},
			      {0.000028081,0.000015884,0.000013473,0.000012691,0.000012659,0.000013430,0.000015598,0.000027889},
			      {0.000027971,0.000015665,0.000013466,0.000012651,0.000012648,0.000013558,0.000016132,0.000029045},
			      {0.000027824,0.000015624,0.000013452,0.000012922,0.000012881,0.000013473,0.000015628,0.000027859},
			      {0.000028053,0.000015657,0.000013501,0.000012726,0.000012889,0.000013432,0.000015494,0.000027716},
			      {0.000028212,0.000015901,0.000013511,0.000012648,0.000012674,0.000013507,0.000015666,0.000027969}};
  rochcor_f mcor_maAer[8][8]={{0.000028957,0.000015643,0.000013419,0.000012634,0.000012700,0.000013592,0.000016042,0.000028597},
			      {0.000027958,0.000015560,0.000013545,0.000012820,0.000012798,0.000013524,0.000015725,0.000027844},
			      {0.000027910,0.000015737,0.000013522,0.000012785,0.000012761,0.000013554,0.000015626,0.000027776},
			      {0.000028081,0.000015884,0.000013473,0.000012691,0.000012659,0.000013430,0.000015598,0.000027889},
			      {0.000027971,0.000015665,0.000013466,0.000012651,0.000012648,0.000013558,0.000016132,0.000029045},
			      {0.000027824,0.000015624,0.000013452,0.000012922,0.000012881,0.000013473,0.000015628,0.000027859},
			      {0.000028053,0.000015657,0.000013501,0.000012726,0.000012889,0.000013432,0.000015494,0.000027716},
			      {0.000028212,0.000015901,0.000013511,0.000012648,0.000012674,0.000013507,0.000015666,0.000027969}};
  
  
  //=======================================================================================================

  rochcor_f dmavgA[8][8]={{0.025932570,0.025091891,0.025007778,0.025482393,0.025525155,0.024942163,0.025240762,0.026173529},
			  {0.025761555,0.025037414,0.025032785,0.025467977,0.025430084,0.024948166,0.025183124,0.025683767},
			  {0.025925532,0.025277217,0.025056270,0.025486077,0.025409136,0.025096012,0.025197756,0.025643884},
			  {0.026097255,0.025228596,0.025005032,0.025495674,0.025503464,0.024999321,0.025117551,0.025903951},
			  {0.025662450,0.025183440,0.025059999,0.025284957,0.025433690,0.025063101,0.025217929,0.025828523},
			  {0.025775122,0.025181569,0.025078492,0.025395508,0.025359363,0.025023036,0.025125309,0.025830371},
			  {0.025922293,0.025114731,0.025089336,0.025389050,0.025431878,0.024966391,0.025124835,0.025982535},
			  {0.025786830,0.025133828,0.025021021,0.025422668,0.025444593,0.025033373,0.025145699,0.025813616}};
  rochcor_f dpavgA[8][8]={{0.025933845,0.025274874,0.024910331,0.025483034,0.025393315,0.025028726,0.025225854,0.026107097},
			  {0.025562564,0.025049116,0.024907495,0.025533540,0.025482513,0.024971331,0.025195101,0.025770362},
			  {0.025906800,0.025134034,0.024994916,0.025461078,0.025443127,0.025040099,0.025197429,0.025935184},
			  {0.026065765,0.025318964,0.024968940,0.025471877,0.025446624,0.025018835,0.025152196,0.025830863},
			  {0.026114878,0.025235284,0.024870686,0.025477622,0.025486434,0.025075934,0.025229683,0.025830525},
			  {0.026041256,0.025286201,0.025054141,0.025462429,0.025505424,0.025089433,0.025266435,0.025770681},
			  {0.025961179,0.025276802,0.024986221,0.025425326,0.025450926,0.025024649,0.025343908,0.025975381},
			  {0.026023325,0.025355806,0.024913144,0.025358313,0.025437377,0.025017029,0.025278262,0.025961934}};
  rochcor_f mmavgA[8][8]={{0.025856714,0.025175208,0.025030680,0.025403673,0.025410328,0.025009167,0.025209394,0.025897363},
			  {0.025857848,0.025114771,0.025014611,0.025502915,0.025487232,0.025017341,0.025148146,0.025864788},
			  {0.025840085,0.025162897,0.025032483,0.025494279,0.025494987,0.025041705,0.025112527,0.025773750},
			  {0.025825612,0.025219687,0.025022809,0.025454062,0.025448556,0.025003955,0.025188739,0.025819642},
			  {0.025839625,0.025189856,0.025022738,0.025399616,0.025469959,0.025027592,0.025179870,0.025901631},
			  {0.025668451,0.025135620,0.025025182,0.025449381,0.025447717,0.025030379,0.025157273,0.025878556},
			  {0.025770836,0.025153959,0.025019239,0.025441799,0.025444473,0.025024552,0.025118440,0.025773079},
			  {0.025883546,0.025189620,0.025004431,0.025441105,0.025438857,0.025050982,0.025140887,0.025747951}};
  rochcor_f mpavgA[8][8]={{0.025848104,0.025165147,0.025012089,0.025427030,0.025462739,0.025013146,0.025244476,0.025954540},
			  {0.025835328,0.025141282,0.024993672,0.025503121,0.025480541,0.025012873,0.025125357,0.025852990},
			  {0.025897269,0.025211229,0.025006486,0.025492131,0.025494318,0.025018339,0.025158256,0.025885241},
			  {0.025936462,0.025184183,0.025000302,0.025466511,0.025453501,0.025027290,0.025191994,0.025985167},
			  {0.025930104,0.025190157,0.024976046,0.025436016,0.025447155,0.025042361,0.025197006,0.025900047},
			  {0.025992957,0.025209131,0.025037077,0.025487890,0.025476201,0.025007556,0.025204365,0.025898344},
			  {0.026044970,0.025213882,0.025026630,0.025466081,0.025496376,0.025031998,0.025197757,0.025925591},
			  {0.025926826,0.025196935,0.025036379,0.025469691,0.025459626,0.025010541,0.025227656,0.026014168}};
  
  //=======================================================================================================
  
  rochcor_f dcor_bfB[8][8]={{-0.000144596,-0.000057628,-0.000017270,-0.000015621,-0.000028382,-0.000006649,-0.000048557,-0.000044299},
			    {-0.000090413,-0.000068176,-0.000041516,-0.000015735,-0.000031943,-0.000028874,-0.000060369,-0.000033126},
			    {-0.000007550,-0.000055456,-0.000034697,-0.000031367,-0.000031080,-0.000029580,-0.000046197,-0.000028467},
			    {0.000004790,-0.000032071,-0.000019090,-0.000017942,-0.000055776,-0.000044560,-0.000067013,-0.000055768},
			    {-0.000051878,-0.000051190,-0.000019297,-0.000023526,-0.000013304,-0.000028762,-0.000041709,-0.000092793},
			    {-0.000072124,-0.000071697,-0.000017282,-0.000001860,0.000002297,-0.000017025,-0.000015171,-0.000016036},
			    {-0.000037174,-0.000079039,-0.000029804,-0.000004805,-0.000006305,-0.000004299,-0.000038990,0.000011080},
			    {-0.000055849,-0.000081118,-0.000030448,-0.000028965,0.000011795,-0.000003969,-0.000015348,-0.000026693}};
  rochcor_f dcor_maB[8][8]={{0.000679506,0.000165445,-0.000161988,-0.000204733,-0.000157694,-0.000121045,0.000341082,0.001310001},
			    {0.001124674,0.000230040,-0.000087382,-0.000081966,-0.000070933,-0.000046097,0.000118593,0.000394306},
			    {0.000236824,0.000045045,0.000032506,0.000053533,0.000052773,0.000055676,0.000079457,0.000416902},
			    {-0.001551573,-0.000359854,0.000025113,0.000105817,0.000154181,0.000167090,0.000039723,0.000151239},
			    {-0.001938668,-0.000417166,0.000022480,0.000079068,0.000175164,0.000274793,0.000124133,-0.000362697},
			    {-0.001242433,-0.000328170,-0.000024177,0.000055492,0.000033491,0.000079773,0.000127969,-0.000188335},
			    {-0.000011573,-0.000110955,-0.000065048,-0.000035398,-0.000070674,-0.000089251,0.000023275,0.001370274},
			    {0.000312706,0.000030575,-0.000150607,-0.000150742,-0.000145880,-0.000199020,0.000149178,0.001677698}};
  rochcor_f mcor_bfB[8][8]={{-0.000098583,-0.000069898,-0.000007832,0.000008182,0.000007505,-0.000001783,-0.000081221,-0.000115174},
			    {-0.000090298,-0.000037332,-0.000004018,-0.000017985,-0.000022153,-0.000006167,-0.000020458,-0.000041471},
			    {-0.000070335,-0.000053552,-0.000013671,-0.000012185,-0.000020384,-0.000031061,-0.000021272,-0.000084478},
			    {-0.000081922,-0.000041818,0.000008209,0.000003445,-0.000003610,-0.000007640,-0.000047846,-0.000078687},
			    {-0.000063307,-0.000019631,0.000006278,0.000010191,-0.000001545,-0.000013677,-0.000035062,-0.000068196},
			    {-0.000025751,-0.000057472,0.000009361,0.000023878,0.000017984,-0.000003890,-0.000037255,-0.000090969},
			    {-0.000097561,-0.000064155,-0.000006429,0.000030402,0.000024613,-0.000001000,-0.000056391,-0.000086373},
			    {-0.000068754,-0.000048149,-0.000005419,0.000004584,-0.000001317,-0.000003972,-0.000023136,-0.000090905}};
  rochcor_f mcor_maB[8][8]={{0.000938268,0.000112741,-0.000164060,-0.000186461,-0.000168353,-0.000201289,0.000016351,0.000148309},
			    {0.001051863,-0.000000449,-0.000116859,-0.000110892,-0.000095532,-0.000059662,0.000199441,0.000763517},
			    {0.000498922,-0.000027857,0.000024765,0.000024700,0.000023201,0.000057399,0.000081967,0.000031988},
			    {0.000681831,0.000412985,0.000181906,0.000133809,0.000152628,0.000166039,0.000325621,0.000333330},
			    {0.000858894,0.000498639,0.000202668,0.000159408,0.000165221,0.000156037,0.000321980,0.000956793},
			    {-0.001197856,-0.000142624,0.000115437,0.000107780,0.000106257,0.000065936,0.000133790,0.000793303},
			    {-0.001701417,-0.000304576,-0.000025958,-0.000016826,-0.000018148,-0.000043827,-0.000138331,-0.000243399},
			    {0.000403774,0.000099571,-0.000129245,-0.000123991,-0.000117905,-0.000153442,-0.000780518,-0.002166593}};
  
  
  rochcor_f dcor_bfBer[8][8]={{0.000073392,0.000037888,0.000030941,0.000028542,0.000028942,0.000031034,0.000038166,0.000071498},
			      {0.000067919,0.000036855,0.000030957,0.000028460,0.000028904,0.000030892,0.000036525,0.000067771},
			      {0.000069890,0.000037252,0.000030693,0.000028233,0.000028295,0.000030924,0.000036761,0.000067732},
			      {0.000068865,0.000037515,0.000030440,0.000028521,0.000028758,0.000031061,0.000036545,0.000067957},
			      {0.000070287,0.000038039,0.000031099,0.000028541,0.000028535,0.000030889,0.000038192,0.000073368},
			      {0.000070529,0.000036023,0.000030708,0.000029111,0.000029156,0.000030485,0.000037271,0.000069426},
			      {0.000068987,0.000036834,0.000030454,0.000028355,0.000028894,0.000030568,0.000036321,0.000069434},
			      {0.000069238,0.000037352,0.000030916,0.000028682,0.000028282,0.000030943,0.000037054,0.000068690}};
  
  rochcor_f dcor_maBer[8][8]={{0.000073392,0.000037888,0.000030941,0.000028542,0.000028942,0.000031034,0.000038166,0.000071498},
			      {0.000067919,0.000036855,0.000030957,0.000028460,0.000028904,0.000030892,0.000036525,0.000067771},
			      {0.000069890,0.000037252,0.000030693,0.000028233,0.000028295,0.000030924,0.000036761,0.000067732},
			      {0.000068865,0.000037515,0.000030440,0.000028521,0.000028758,0.000031061,0.000036545,0.000067957},
			      {0.000070287,0.000038039,0.000031099,0.000028541,0.000028535,0.000030889,0.000038192,0.000073368},
			      {0.000070529,0.000036023,0.000030708,0.000029111,0.000029156,0.000030485,0.000037271,0.000069426},
			      {0.000068987,0.000036834,0.000030454,0.000028355,0.000028894,0.000030568,0.000036321,0.000069434},
			      {0.000069238,0.000037352,0.000030916,0.000028682,0.000028282,0.000030943,0.000037054,0.000068690}};
  
  rochcor_f mcor_bfBer[8][8]={{0.000031813,0.000016534,0.000013999,0.000013046,0.000013123,0.000014245,0.000016964,0.000031719},
			      {0.000030718,0.000016483,0.000014127,0.000013244,0.000013198,0.000014151,0.000016711,0.000030863},
			      {0.000030665,0.000016682,0.000014126,0.000013207,0.000013188,0.000014194,0.000016551,0.000030725},
			      {0.000030997,0.000016830,0.000014015,0.000013110,0.000013070,0.000014090,0.000016538,0.000030930},
			      {0.000030883,0.000016557,0.000014048,0.000013105,0.000013067,0.000014169,0.000017090,0.000032174},
			      {0.000030553,0.000016523,0.000014045,0.000013333,0.000013320,0.000014083,0.000016591,0.000030854},
			      {0.000030886,0.000016615,0.000014077,0.000013136,0.000013316,0.000014054,0.000016452,0.000030541},
			      {0.000031052,0.000016845,0.000014093,0.000013053,0.000013071,0.000014144,0.000016638,0.000031061}};
  
  rochcor_f mcor_maBer[8][8]={{0.000031813,0.000016534,0.000013999,0.000013046,0.000013123,0.000014245,0.000016964,0.000031719},
			      {0.000030718,0.000016483,0.000014127,0.000013244,0.000013198,0.000014151,0.000016711,0.000030863},
			      {0.000030665,0.000016682,0.000014126,0.000013207,0.000013188,0.000014194,0.000016551,0.000030725},
			      {0.000030997,0.000016830,0.000014015,0.000013110,0.000013070,0.000014090,0.000016538,0.000030930},
			      {0.000030883,0.000016557,0.000014048,0.000013105,0.000013067,0.000014169,0.000017090,0.000032174},
			      {0.000030553,0.000016523,0.000014045,0.000013333,0.000013320,0.000014083,0.000016591,0.000030854},
			      {0.000030886,0.000016615,0.000014077,0.000013136,0.000013316,0.000014054,0.000016452,0.000030541},
			      {0.000031052,0.000016845,0.000014093,0.000013053,0.000013071,0.000014144,0.000016638,0.000031061}};
  
  //=======================================================================================================
  
  rochcor_f dmavgB[8][8]={{0.025991655,0.025311835,0.024988737,0.025242780,0.025371926,0.025065144,0.025324183,0.026152630},
			  {0.025856235,0.025084031,0.025039418,0.025443285,0.025366023,0.024968109,0.025218131,0.025961025},
			  {0.025947911,0.025195929,0.024941870,0.025401292,0.025408318,0.025043408,0.025241994,0.026030413},
			  {0.026163362,0.025252581,0.024993347,0.025320662,0.025405934,0.025023824,0.025256723,0.026043456},
			  {0.026177471,0.025308802,0.025012596,0.025357476,0.025356569,0.025056141,0.025215468,0.025887252},
			  {0.026179659,0.025391513,0.025075739,0.025372621,0.025394723,0.025045877,0.025299906,0.025733539},
			  {0.025898144,0.025156961,0.024936140,0.025271397,0.025341783,0.025005383,0.025236019,0.025943436},
			  {0.026169335,0.025212414,0.025015055,0.025272623,0.025360333,0.025122051,0.025276073,0.026141349}};
  rochcor_f dpavgB[8][8]={{0.026158246,0.025377322,0.024982229,0.025487195,0.025351342,0.024976617,0.025325305,0.026204964},
			  {0.026166274,0.025380258,0.024988199,0.025390884,0.025366336,0.025005322,0.025164644,0.025991795},
			  {0.025970544,0.025259971,0.024972570,0.025345077,0.025323484,0.025089168,0.025305135,0.025868954},
			  {0.026116943,0.025368625,0.024962821,0.025353121,0.025355820,0.024944845,0.025337641,0.025987386},
			  {0.026137895,0.025339877,0.025056626,0.025355330,0.025292533,0.024966852,0.025258951,0.026016475},
			  {0.026075538,0.025291279,0.025011884,0.025318154,0.025422679,0.025037061,0.025338756,0.026069004},
			  {0.026124985,0.025261807,0.024951238,0.025334010,0.025312376,0.025142031,0.025236447,0.026153435},
			  {0.026079515,0.025289200,0.025037722,0.025406404,0.025344141,0.025006644,0.025316962,0.025963792}};
  rochcor_f mmavgB[8][8]={{0.025933069,0.025258618,0.025025516,0.025365401,0.025368327,0.025000757,0.025245661,0.026024967},
			  {0.025910723,0.025157762,0.024992447,0.025427054,0.025414191,0.025013238,0.025176806,0.025904723},
			  {0.025891919,0.025227784,0.025036093,0.025431870,0.025439552,0.025052485,0.025172865,0.025904588},
			  {0.025948493,0.025273462,0.025019432,0.025364943,0.025411198,0.025017116,0.025257625,0.025880222},
			  {0.025935202,0.025231815,0.025017403,0.025378937,0.025409033,0.025043469,0.025259976,0.025996101},
			  {0.025770896,0.025196642,0.025026095,0.025379524,0.025401986,0.025028419,0.025215514,0.025967808},
			  {0.025832932,0.025190771,0.025026971,0.025362232,0.025388534,0.025023211,0.025179547,0.025838882},
			  {0.025993166,0.025212188,0.025007198,0.025368480,0.025397241,0.025035818,0.025219471,0.025820571}};
  rochcor_f mpavgB[8][8]={{0.025910124,0.025230752,0.025008225,0.025389254,0.025416237,0.025033664,0.025288499,0.026027373},
			  {0.025931138,0.025193130,0.025002277,0.025470186,0.025436435,0.024995898,0.025185658,0.025980623},
			  {0.026007213,0.025288190,0.025017625,0.025408528,0.025450779,0.025038394,0.025196966,0.025933806},
			  {0.025965488,0.025247854,0.024980460,0.025397115,0.025371868,0.025027828,0.025228527,0.026101624},
			  {0.026075500,0.025190716,0.024979688,0.025379534,0.025388964,0.025029992,0.025270707,0.025928120},
			  {0.026064307,0.025262637,0.025015625,0.025423208,0.025388981,0.025009200,0.025296653,0.025992580},
			  {0.026152539,0.025266494,0.025025838,0.025408022,0.025436199,0.025037154,0.025265450,0.026029142},
			  {0.025975868,0.025219767,0.025010740,0.025413945,0.025407500,0.025036560,0.025262577,0.026116943}};
  
  //===============================================================================================
  //parameters for Z pt correction
  
  rochcor_f ptlow[85] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5,
			 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5,
			 10.0, 10.5, 11.0, 11.5, 12.0, 12.5, 13.0, 13.5, 14.0, 14.5,
			 15.0, 15.5, 16.0, 16.5, 17.0, 17.5, 18.0, 18.5, 19.0, 19.5,
			 20.0, 20.5, 21.0, 21.5, 22.0, 22.5, 23.0, 23.5, 24.0, 24.5,
			 25.0, 26.0, 27.0, 28.0, 29.0,
			 30.0, 32.0, 34.0, 36.0, 38.0,
			 40.0, 44.0, 48.0, 52.0, 56.0,
			 60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0,
			 100.0, 110.0, 120.0, 130.0, 140.0, 150.0, 175.0,
			 200.0, 250.0, 350.0, 500.0, 1000.0};
  
  //int nptbins( sizeof(ptlow)/sizeof(float) - 1 );
  
  rochcor_f zptscl[84] = {1.49177,1.45654,1.36283,1.28569,1.2418,1.12336,1.10416,1.08731,0.994051,0.96532,
			  0.94427,0.932725,0.918082,0.899665,0.898398,0.927687,0.908047,0.892392,0.924027,0.945895,
			  0.937149,0.923983,0.923387,0.955362,0.947812,0.962943,0.948781,0.961555,0.95222,0.999207,
			  0.973884,0.993013,0.953487,0.951402,0.985583,0.986603,0.981388,1.00022,1.0294,0.964748,
			  0.974592,1.01546,0.992343,1.00101,0.990866,0.98982,1.02924,1.02265,0.967695,1.02411,
			  0.97331,1.01052,1.01561,0.992594,0.976504,1.01205,0.981111,1.00078,1.02078,1.00719,
			  1.0099,1.02865,1.03845,1.03254,1.09815,1.10263,1.06302,1.0725,1.14703,1.10574,
			  1.13911,1.16947,1.1709,1.11413,1.28793,1.18953,1.20212,1.18112,1.25471,1.15329,
			  1.14276,1.17223,1.09173,2.00229};
  
  rochcor_f zptscler[84] = {0.0270027,0.0154334,0.0115338,0.00958085,0.0084683,0.00736665,0.0069567,0.00671434,
			    0.00617693,0.00601943,0.00594735,0.00594569,0.00594903,0.00595495,0.00608115,0.00633704,
			    0.0063916,0.0064468,0.00678106,0.00706769,0.00717517,0.00727958,0.00747182,0.00785544,
			    0.00798754,0.00828787,0.00839147,0.00865826,0.00876775,0.00933276,0.00935768,0.0097289,
			    0.00962058,0.00983828,0.0103044,0.0104871,0.0106575,0.0110388,0.0114986,0.0111494,
			    0.0115202,0.0121059,0.0121345,0.0124923,0.0125972,0.0128401,0.0134519,0.0136279,
			    0.0133414,0.014186,0.00992195,0.0105984,0.0109484,0.0111756,0.0114579,0.00870013,
			    0.00904749,0.00970734,0.0104583,0.0109818,0.00837852,0.00939894,0.010415,0.0113433,
			    0.013007,0.0128788,0.0140174,0.0156993,0.0181717,0.019765,0.0222326,0.0249408,
			    0.0272806,0.0211706,0.0278087,0.0306654,0.0361387,0.041327,0.0341513,0.0440116,
			    0.0473006,0.0680212,0.149162,0.56279};

  RochesterCorrections2011::~RochesterCorrections2011(){
  }

  RochesterCorrections2011::RochesterCorrections2011(bool central_value){

    rng = edm::Service<edm::RandomNumberGenerator>();
    if ( ! rng.isAvailable()) {
      throw cms::Exception("Configuration")
	<< "XXXXXXX requires the RandomNumberGeneratorService\n"
	"which is not present in the configuration file.  You must add the service\n"
	"in the configuration file or remove the modules that require it.";
    }

    CLHEP::RandGaussQ init(rng->getEngine(), 0, 1.0);

    gscler_mc_dev=(!central_value)*init.fire();
    gscler_da_dev=(!central_value)*init.fire();

    for(int i=0; i<8; ++i){
      for(int j=0; j<8; ++j){
	mptsys_mc_dm[i][j]=(!central_value)*init.fire();
	mptsys_mc_da[i][j]=(!central_value)*init.fire();
	mptsys_da_dm[i][j]=(!central_value)*init.fire();
	mptsys_da_da[i][j]=(!central_value)*init.fire();
      }
    }
  }

  
  void RochesterCorrections2011::momcor_mc( TLorentzVector& mu, float charge, float sysdev, int runopt, float& qter){
    
    //sysdev == num : deviation = num
    
    float ptmu = mu.Pt();
    float muphi = mu.Phi();
    float mueta = mu.Eta(); // same with mu.Eta() in Root
    
    float px = mu.Px();
    float py = mu.Py();
    float pz = mu.Pz();
    float e = mu.E();
    
    int mu_phibin = phibin(muphi);
    int mu_etabin = etabin(mueta);
    
    //float mptsys = sran.Gaus(0.0,sysdev);
    
    float dm = 0.0;
    float da = 0.0;
    
    if(runopt == 0){
      dm = (mcor_bfA[mu_phibin][mu_etabin] + mptsys_mc_dm[mu_phibin][mu_etabin]*mcor_bfAer[mu_phibin][mu_etabin])/mmavgA[mu_phibin][mu_etabin];
      da = mcor_maA[mu_phibin][mu_etabin] + mptsys_mc_da[mu_phibin][mu_etabin]*mcor_maAer[mu_phibin][mu_etabin];
    }else if(runopt == 1){
      dm = (mcor_bfB[mu_phibin][mu_etabin] + mptsys_mc_dm[mu_phibin][mu_etabin]*mcor_bfBer[mu_phibin][mu_etabin])/mmavgB[mu_phibin][mu_etabin];
      da = mcor_maB[mu_phibin][mu_etabin] + mptsys_mc_da[mu_phibin][mu_etabin]*mcor_maBer[mu_phibin][mu_etabin];
    }
    
    float cor = 1.0/(1.0 + dm + charge*da*ptmu);
    
    //for the momentum tuning - eta,phi,Q correction
    px *= cor;
    py *= cor;
    pz *= cor;
    e  *= cor;
    
    float recm = 0.0;
    float drecm = 0.0; 
    float delta = 0.0;
    float sf = 0.0;
    
    float gscler = 0.0;
    float deltaer = 0.0;
    float sfer = 0.0;
    
    if(runopt==0){
      recm = recmA;
      drecm = drecmA;
      
      delta = deltaA;
      sf = sfA;
      
      gscler = TMath::Sqrt( TMath::Power(mgsclA_stat,2) + TMath::Power(mgsclA_syst,2) );
      deltaer = TMath::Sqrt( TMath::Power(deltaA_stat,2) + TMath::Power(deltaA_syst,2) );
      sfer = TMath::Sqrt( TMath::Power(sfA_stat,2) + TMath::Power(sfA_syst,2) );
    }else if(runopt==1){
      recm = recmB;
      drecm = drecmB;
      
      delta = deltaB;
      sf = sfB;
      
      gscler = TMath::Sqrt( TMath::Power(mgsclB_stat,2) + TMath::Power(mgsclB_syst,2) );
      deltaer = TMath::Sqrt( TMath::Power(deltaB_stat,2) + TMath::Power(deltaB_syst,2) );
      sfer = TMath::Sqrt( TMath::Power(sfB_stat,2) + TMath::Power(sfB_syst,2) );
    }
    
    float gscl = (genm_smr/recm);
    
    px *= (gscl + gscler_mc_dev*gscler);
    py *= (gscl + gscler_mc_dev*gscler);
    pz *= (gscl + gscler_mc_dev*gscler);
    e  *= (gscl + gscler_mc_dev*gscler);
    
    float momscl = sqrt(px*px + py*py)/ptmu;
    
    CLHEP::RandGaussQ smear(rng->getEngine(), 1.0, sf + sysdev*sfer);

    float tune = 1.0/(1.0 + (delta + sysdev*deltaer)*sqrt(px*px + py*py)*smear.fire());
    
    px *= (tune); 
    py *= (tune);  
    pz *= (tune);  
    e  *= (tune);   
    
    qter *= (momscl*momscl + (1.0-tune)*(1.0-tune));
    
    mu.SetPxPyPzE(px,py,pz,e);
  
}


void RochesterCorrections2011::momcor_data( TLorentzVector& mu, float charge, float sysdev, int runopt, float& qter){
  
  float ptmu = mu.Pt();

  float muphi = mu.Phi();
  float mueta = mu.Eta(); // same with mu.Eta() in Root

  float px = mu.Px();
  float py = mu.Py();
  float pz = mu.Pz();
  float e = mu.E();
  
  int mu_phibin = phibin(muphi);
  int mu_etabin = etabin(mueta);
  
  //float mptsys1 = sran.Gaus(0.0,sysdev);
  
  float dm = 0.0;
  float da = 0.0;
  
  if(runopt==0){
    dm = (dcor_bfA[mu_phibin][mu_etabin] + mptsys_da_dm[mu_phibin][mu_etabin]*dcor_bfAer[mu_phibin][mu_etabin])/dmavgA[mu_phibin][mu_etabin];
    da = dcor_maA[mu_phibin][mu_etabin] + mptsys_da_da[mu_phibin][mu_etabin]*dcor_maAer[mu_phibin][mu_etabin];
  }else if(runopt==1){
    dm = (dcor_bfB[mu_phibin][mu_etabin] + mptsys_da_dm[mu_phibin][mu_etabin]*dcor_bfBer[mu_phibin][mu_etabin])/dmavgB[mu_phibin][mu_etabin];
    da = dcor_maB[mu_phibin][mu_etabin] + mptsys_da_da[mu_phibin][mu_etabin]*dcor_maBer[mu_phibin][mu_etabin];
  }
  
  float cor = 1.0/(1.0 + dm + charge*da*ptmu);
  
  px *= cor;
  py *= cor;
  pz *= cor;
  e  *= cor;
  
  //after Z pt correction
  float recm = 0.0;
  float gscler = 0.0;
  
  if(runopt==0){
    recm = drecmA;
    gscler = TMath::Sqrt( TMath::Power(dgsclA_stat,2) + TMath::Power(dgsclA_syst,2) );
  }else if(runopt==1){
    recm = drecmB;
    gscler = TMath::Sqrt( TMath::Power(dgsclB_stat,2) + TMath::Power(dgsclB_syst,2) );
  }
  
  float gscl = (genm_smr/recm);
  
  px *= (gscl + gscler_da_dev*gscler);
  py *= (gscl + gscler_da_dev*gscler);
  pz *= (gscl + gscler_da_dev*gscler);
  e  *= (gscl + gscler_da_dev*gscler);
  
  float momscl = sqrt(px*px + py*py)/ptmu;
  qter *= momscl;

  mu.SetPxPyPzE(px,py,pz,e);
  
}

void RochesterCorrections2011::musclefit_data( TLorentzVector& mu, TLorentzVector& mubar){

  float dpar1 = 0.0;
  float dpar2 = 0.0;
  float epar1 = 0.0;
  float epar2 = 0.0;
  
  if(fabs(mu.PseudoRapidity())<=0.9){
    dpar1 = d0par;
    epar1 = e0par;
  }else if(mu.PseudoRapidity()>0.9){
    dpar1 = d1par;
    epar1 = e1par;
  }else if(mu.PseudoRapidity()<-0.9){
    dpar1 = d2par;
    epar1 = e2par;
  }

  if(fabs(mubar.PseudoRapidity())<=0.9){
    dpar2 = d0par;
    epar2 = e0par;
  }else if(mubar.PseudoRapidity()>0.9){
    dpar2 = d1par;
    epar2 = e1par;
  }else if(mubar.PseudoRapidity()<-0.9){
    dpar2 = d2par;
    epar2 = e2par;
  }

  float corr1 = 1.0 + bpar*mu.Pt() + (-1.0)*cpar*mu.Pt()*TMath::Sign(float(1.0),float(mu.PseudoRapidity()))*TMath::Power(mu.PseudoRapidity(),2)
    + (-1.0)*dpar1*mu.Pt()*sin(mu.Phi() + epar1);
  float corr2 = 1.0 + bpar*mubar.Pt() + (1.0)*cpar*mubar.Pt()*TMath::Sign(float(1.0),float(mubar.PseudoRapidity()))*TMath::Power(mubar.PseudoRapidity(),2)
    + (1.0)*dpar2*mubar.Pt()*sin(mubar.Phi() + epar2);
  
  float px1 = mu.Px();
  float py1 = mu.Py();
  float pz1 = mu.Pz();
  float e1 = mu.E();
  
  float px2 = mubar.Px();
  float py2 = mubar.Py();
  float pz2 = mubar.Pz();
  float e2 = mubar.E();

  px1 *= corr1;
  py1 *= corr1;
  pz1 *= corr1;
  e1 *= corr1;
  
  px2 *= corr2;
  py2 *= corr2;
  pz2 *= corr2;
  e2 *= corr2;
  
  mu.SetPxPyPzE(px1,py1,pz1,e1);
  mubar.SetPxPyPzE(px2,py2,pz2,e2);

}

Int_t RochesterCorrections2011::phibin(float phi){
  
  int nphibin = -1;
  
  for(int i=0; i<8; i++){
    if(-pi+(2.0*pi/8.0)*i <= phi && -pi+(2.0*pi/8.0)*(i+1) > phi){
      nphibin = i;
      break;
    }
  }
  
  return nphibin;
}

Int_t RochesterCorrections2011::etabin(float eta){

  int nbin = -1;
  
  for(int i=0; i<8; i++){
    if(netabin[i] <= eta && netabin[i+1] > eta){
      nbin = i;
      break;
    }
  }
  
  return nbin;
}

float RochesterCorrections2011::zptcor(float gzpt) {
  int ibin( 0 );
  
  // mcptscl[] = 84 bins: [0] and [83] are the underflow and overflow
  if ( gzpt > ptlow[nptbins] ) return nptbins-1;
  if ( gzpt < ptlow[0      ] ) return 0;
  
  for ( int i=0; i<nptbins; ++i ) {
    if ( gzpt>=ptlow[i] && gzpt<ptlow[i+1] ) { ibin=i; break; }
  }

  float zptwt = zptscl[ibin];

  return zptwt;
}

} // namespace rochcor
