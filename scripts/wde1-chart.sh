
DBHOST=localhost

while [ 1 ]; do

  wde1-chart tempKombi 6 /tmp/chg_w0.jpg $DBHOST
  wde1-chart humKombi  6 /tmp/chg_w1.jpg $DBHOST
  wde1-chart windKombi 6 /tmp/chg_w2.jpg $DBHOST
  # wde1-chart rainvolKombi 6 /tmp/chg_w3.jpg $DBHOST

  sleep 300

done


