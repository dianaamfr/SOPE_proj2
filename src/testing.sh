#!/bin/bash
RECVD=`grep RECVD q1.log | wc -l` ; echo "RECVD: $RECVD";
IWANT=`grep IWANT u1.log | wc -l` ; echo "IWANT: $IWANT";
echo "----------------";
ENTER=`grep ENTER q1.log | wc -l` ; echo "ENTER: $ENTER";
IAMIN=`grep IAMIN u1.log | wc -l` ; echo "IAMIN: $IAMIN";
echo "----------------";
n2LATE=`grep 2LATE q1.log | wc -l` ; echo "2LATE: $n2LATE";
nCLOSD=`grep CLOSD u1.log | wc -l` ; echo "CLOSD: $nCLOSD";
echo "----------------";
TIMUP=`grep TIMUP q1.log | wc -l` ; echo "TIMUP: $TIMUP";
echo "----------------";
FAILD=`grep FAILD u1.log | wc -l` ; echo "FAILD: $FAILD";
echo "----------------";
GAVUP=`grep GAVUP q1.log | wc -l` ; echo "GAVUP: $GAVUP";