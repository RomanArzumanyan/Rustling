#include <iostream>
#include <vector>
#include "Rustling.hpp"

using namespace rustling;
using namespace std;

int main()
{
    PlatformMgr plt_mgr;
    Platform my = plt_mgr.makePlatform(CL_DEVICE_TYPE_GPU);
    cout <<
         my.getExtensions() << endl <<
         my.getID()         << endl <<
         my.getName()       << endl <<
         my.getProfile()    << endl <<
         my.getVendor()     << endl <<
         my.getVersion()    << endl;

    DeviceMgr dev_mgr;
    Device gpu = dev_mgr.makeDevice(CL_DEVICE_TYPE_GPU);
    cout << gpu.getName() << endl;

    Device cpu = dev_mgr.makeDevice(CL_DEVICE_TYPE_CPU);
    cout << cpu.getName() << endl;

    Context ctx({cpu, gpu});

    CmdQueue queue(ctx, gpu);
    try {
        queue << rustling::flush << rustling::finish;
        rustling::flush(queue);
    } catch (SException &e) {
        cout << e.what() << endl;
    }

    vector<int> vec({0, 1, 2, 3, 4});

    Event evt;
    Buffer<int> buf(ctx, vec.size(), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, &vec[0]);
    queue << (buf.map() >> evt);
    evt.Wait();

    cout << buf[0] << "\t" << buf[1] << "\t" << buf[2] << "\t" << buf[3] << endl;
    cout << evt.getTime() << endl;
    cout << evt.isUsed() << endl;

    try {
        queue << buf.read(&vec[0])(true) << buf.write(&vec[0])(true);

        Event evt1;
        queue << (buf.unmap() >> evt1);
        evt1.Wait();
        cout << evt1.getTime() << endl;

        Event evt2;
        Kernel my_kernel(ctx, gpu, "My", "/home/roman/kernel.cl");
        queue << (my_kernel({32, 1}, {}, {}) >> evt2);

        evt2.Wait();
    } catch (SException &e) {
        cout << e.what() << endl;
    }

    return 0;
}
