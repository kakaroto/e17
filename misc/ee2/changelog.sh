echo `date` >> ChangeLog.1
echo "($LOGNAME)" >> ChangeLog.1
echo "" >> ChangeLog.1
echo "" >> ChangeLog.1
echo "" >> ChangeLog.1
echo "-------------------------------------------------------------------------------" >> ChangeLog.1
echo "" >> ChangeLog.1
jed ChangeLog.1
mv ChangeLog ChangeLog.2
cat ChangeLog.2 ChangeLog.1 > ChangeLog
rm ChangeLog.1 
rm ChangeLog.2
rm *~
echo ""
echo "Done."
