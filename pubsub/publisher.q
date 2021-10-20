\d .testdata

// set the port
@[system;"p 6055";{-2"Failed to set port to 6812: ",x,
	 	     ". Please ensure no other processes are running on that port",
		     " or change the port in both the publisher and subscriber scripts.";  
		     exit 1}]

\d .

// the tables to be published - all must be in the top level namespace
// tables to be published require a sym column, which can be of any type
// apart from that, they can be anything you like
ivol:([] time:`timespan$();cnt:`long$();sym:`$();fairsynp:`float$();cp:();strike:();vol:());
ivolt:([] cnt:`long$();sym:`$();fairsynp:`float$();vol:());
ivolz:([] cnt:`long$();sym:`$();fairsynp:`float$());
pubdata:{(-1*x)#ivolt}

// load in u.q from tick
upath:"kdb-tick/tick/u.q"
@[system;"l ",upath;{-2"Failed to load u.q from ",x," : ",y, 
		       ". Please make sure u.q is accessible.",
                       " kdb+tick can be downloaded from http://code.kx.com/wsvn/code/kx/kdb+tick";
		       exit 2}[upath]]

// initialise pubsub 
// all tables in the top level namespace (`.) become publish-able
// tables that can be published can be seen in .u.w
.u.init[];
//.u.upd:{[t;d] t insert ("n"$.z.P),d;};
.u.upd:{[t;d] t insert .z.N,d;};
// functions to publish data
// .u.pub takes the table name and table data
// there is no checking to ensure that the table being published matches
// the table schema defined at the top level
// that is left up to the programmer!
publishmeter:{.u.pub[`ivolt;pubdata[x]]}

// create timer function to randomly publish
// between 1 and 10 meter records, and between 1 and 5 grid records
.z.ts:{publishmeter[1+rand 10]}

/- fire timer every 1 second
\t 1000
