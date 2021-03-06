
#include "bench.h"
#include <CORBA.h>
#include <iostream>
#include <string>


using namespace std;

bench_ptr bench;
int num;
int delay;
int thread_number;
string method = "";
CORBA::Context_var ctx, ctx2;

#ifdef HAVE_THREADS

class Notifier : public CORBA::RequestCallback {
private:
    MICOMT::Locked<std::list<CORBA::Request_var> > requests;
public:
    void add_request (CORBA::Request_ptr r) {
        MICOMT::AutoLock l(requests);
        cerr << "add_request: " << requests.size() << endl;
        requests.push_back(CORBA::Request::_duplicate(r));
    }
    void callback (CORBA::Request_ptr r, CORBA::RequestCallback::Event ev) {
        cerr << "callback." << endl;
        assert (ev == CORBA::RequestCallback::RequestDone);
        MICOMT::AutoLock lock(requests);
        list<CORBA::Request_var>::iterator j = requests.begin();
        for (; j != requests.end(); ++j) {
            CORBA::Request_ptr req = (*j).in();
            if (r == req) {
                MICO_CATCHANY (req->get_response ());
                if (req->env()->exception()) {
                    cerr << "eventd: push failed with: "
		     << req->env()->exception() << endl;
                }
                cerr << "erasing req: " << (void*)req << " from queue size: " << requests.size() << endl;
                requests.erase (j);
                break;
            }
        }
    }

};

class Invoker
    : virtual public MICOMT::Thread
{
public:
    void _run(void*)
    {
	if (method == "perform") {
	    for (int i=0; i<num; i++) {
		bench->perform();
	    }
	} else if (method == "perform_oneway") {
	    for (int i=0; i<num; i++) {
		bench->perform_oneway();
	    }
	} else if (method == "perform_with_context") {
	    for (int i=0; i<num; i++) {
		bench->perform_with_context(ctx2);
	    }
	} else if (method == "perform_oneway_with_context") {
	    for (int i=0; i<num; i++) {
		bench->perform_oneway_with_context(ctx2);
	    }
	} else if (method == "non_existent") {
	    for (int i=0; i<num; i++) {
		bench->_non_existent();
	    }
	} else {
	    assert(0);
	}
    }
};

#endif // HAVE_THREADS

int
main (int argc, char* argv[])
{
try {
    CORBA::ORB_ptr orb = CORBA::ORB_init (argc, argv, "mico-local-orb");
  
    cout << "argv[0]: " << argv[0] << endl;
    cout << "argv[1]: " << argv[1] << endl;
    cout << "argv[2]: " << argv[2] << endl;
    cout << "argv[3]: " << argv[3] << endl;
    cout << "argv[4]: " << argv[4] << endl;
    cout << "argv[5]: " << argv[5] << endl;

    string s = argv[1];
    method = argv[2];
    cout << "binding: " << s << endl;
    if (s.find("bind") != string::npos) {
	cout << "binding..." << endl;
        size_t pos = -1;
	CORBA::Object_ptr obj = CORBA::Object::_nil();
        if ((pos = s.find("#")) != string::npos) {
            string tag;
            tag = s.substr(pos + 1, s.size());
            cout << "tag: `" + tag << "'" << endl;
            CORBA::ORB::ObjectTag_var t = CORBA::ORB::string_to_tag(tag.c_str());
            obj = orb->bind ("IDL:bench:1.0", t, "inet:localhost:7788");
        }
        else {
            obj = orb->bind ("IDL:bench:1.0");
        }
	bench = bench::_narrow (obj);
	cout << "binded." << endl;
    }
    else if (s == "ior") {
	string ior;
	cin >> ior;
	CORBA::Object_ptr obj = orb->string_to_object(ior.c_str());
	bench = bench::_narrow (obj);
    }
    else {
	cerr << "unsupported binding method." << endl;
	exit(2);
    }
    if (CORBA::is_nil(bench)) {
	cerr << "can't bind to the bench interface" << endl;
	exit(1);
    }
    cout << "operation: " << method << endl;
    num = atoi (argv[3]);
    cout << "num: " << num << endl;
    int tnum = num / 100;
    cout << "tnum: " << tnum << endl;
    cout << "invocation: " << tnum*100 << endl;

    delay = atoi (argv[4]);
    cout << "invoke delay: " << delay << endl;

    CORBA::Any any;
    orb->get_default_context( ctx );

    ctx->create_child ("child", ctx2);

    any <<= "aa";
    ctx->set_one_value ("aa", any);

    any <<= "ab";
    ctx->set_one_value ("ab", any);

    any <<= "bb";
    ctx->set_one_value ("bb", any);

    any <<= "aa-child";
    ctx2->set_one_value ("aa", any);

#ifdef HAVE_THREADS
    thread_number = atoi (argv[5]);
    cout << "number of threads: " << thread_number << endl;
    if (thread_number > 0) {
	Invoker** thr_array = new Invoker*[thread_number];
	for (int i=0; i<thread_number; i++) {
	    thr_array[i] = new Invoker;
	}
	for (int i=0; i<thread_number; i++) {
	    thr_array[i]->start();
	}
	for (int i=0; i<thread_number; i++) {
	    thr_array[i]->wait();
	}
	//bench->shutdown();
	orb->destroy();
	return 0;
    }
#endif // HAVE_THREADS
    if (method == "perform") {
	for (int j=0; j<100; j++) {
	    cout << "\r" << "progress: " << j << "%" << flush;
	    for (int i=0; i<tnum; i++) {
		bench->perform();
		//::sleep (delay);
	    }
	}
    } else if (method == "perform_oneway") {
	for (int j=0; j<100; j++) {
	    cout << "\r" << "progress: " << j << "%" << flush;
	    for (int i=0; i<tnum; i++) {
                bench->perform_oneway();
		//::sleep (delay);
	    }
	}
    } else if (method == "perform_with_context") {
	for (int j=0; j<100; j++) {
	    cout << "\r" << "progress: " << j << "%" << flush;
	    for (int i=0; i<tnum; i++) {
                bench->perform_with_context(ctx2);
		//::sleep (delay);
	    }
	}
    } else if (method == "perform_oneway_with_context") {
	for (int j=0; j<100; j++) {
	    cout << "\r" << "progress: " << j << "%" << flush;
	    for (int i=0; i<tnum; i++) {
                bench->perform_oneway_with_context(ctx2);
		//::sleep (delay);
	    }
	}
    }
    else if (method == "non_existent") {
	for (int j=0; j<100; j++) {
	    cout << "\r" << "progress: " << j << "%" << flush;
	    for (int i=0; i<tnum; i++) {
                bench->_non_existent();
		//::sleep (delay);
	    }
        }
    }
    else if (method == "long_seq") {
	CORBA::ULong len = atoi(argv[6]);
	cerr << "seq len: " << len << endl;
	bench::LongSeq x;
	bench::LongSeq_var ret;
	x.length(len);
	for (int j=0; j<100; j++) {
	    cout << "\r" << "progress: " << j << "%" << flush;
	    for (int i=0; i<tnum; i++) {
                ret = bench->long_seq(x);
		//::sleep (delay);
	    }
	}
    }
    else if (method == "send_deferred") {
        Notifier* notif = new Notifier();
	for (int j=0; j<100; j++) {
	    cout << "\r" << "progress: " << j << "%" << flush;
	    for (int i=0; i<tnum; i++) {
                CORBA::Request_var req = bench->_request("perform");
                notif->add_request(req);
                req->send_deferred(notif);
                req->poll_response();
            }
        }
        while (orb->work_pending()) {
            orb->perform_work();
        }
    } else {
	assert(0);
    }
#ifdef USE_MEMTRACE
    MemTrace_Report(stderr);
#endif
    bench->shutdown();
    orb->shutdown(TRUE);
    orb->destroy();
    cout << "client: finished." << endl;
}
catch (const CORBA::Exception& ex) {
    ex._print(cerr);
    cerr << "  exception thrown" << endl;
}
catch (...) {
    cerr << "... unknown exception thrown" << endl;
}
    return 0;
}
