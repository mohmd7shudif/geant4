#/bin/csh

setenv HISTODIR 20101011+PHEVAP
mkdir -p $HISTODIR

setenv PHYSLIST LHEP
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QGSP_BERT_EMV
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QGSP_BERT_EMX
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QGSP_BERT
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QGSP_BIC
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QBBC
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QBBC_XGG
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QBBC_XGGSN
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST FTFP_BERT
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST QGSP_FTFP_BERT
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

setenv PHYSLIST CHIPS
$G4MY/hadr01 p_pb.in >& $HISTODIR/$PHYSLIST.out

echo "Done!"
#
