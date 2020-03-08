#include <deque>
#include <thread>
#include <regex>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
struct report
{
    /// report item
    
    int lineNo;
    std::string::size_type offset;
    std::string value;
};



int main(int argc, char *argv[])
{

    
    /// check commandline params
    if(argc!=3)
    {
        printf("Usage: %s <filename> \"wildcard mask\"", argv[0]);
        return 1;
    }

    std::string searchSample=argv[2];

    /// prepare regexp
    std::string regexStr;
    for(auto& c:searchSample)
    {
        if(c=='?')
            regexStr+="[\\w\\s[:punct:]]{1}";
        else
            regexStr+=c;
    }

    std::regex re("("+regexStr+")");


    /// string processors
    std::vector<std::thread> handlers;
    /// flag used to signal threads to stop. May be used without mutex lock.
    bool done=false;
    
    /// string container
    std::deque<std::pair<int,std::string> > mx_strings_queue;
    /// mutex for string container
    std::mutex mx_strings_queue_mutex;

    /// condition variable used to awake threads
    std::condition_variable cond_var;



    /// reports container
    std::deque<report> mx_reports;
    /// mutex for repors container
    std::mutex mx_reports_mutex;



    /// run 10 threads
    for(int i=0;i<10;i++)
    {
        handlers.push_back(std::thread([&](){

	    
            while(!done)
            {
        	/// extract line element from queue
                std::pair<int,std::string> element;
                bool isLineExtracted=false;
                {
                    std::unique_lock<std::mutex> lock(mx_strings_queue_mutex);
                    
                    /// if container is empty - wait signal from string reader thread (main thread)
                    if(mx_strings_queue.empty())
                    {
                	/// condition wait release mutex for other threads
                        cond_var.wait(lock);
                        /// after wait mutex locked again
                    }
                    if(!mx_strings_queue.empty())
                    {
                        element=mx_strings_queue[0];
                        mx_strings_queue.pop_front();
                        isLineExtracted=true;
                    }
                    /// mutex lock  stops here
                }
                if(isLineExtracted)
                {
                    std::smatch m;
                    std::regex_search(element.second, m, re);
                    if(!m.empty())
                    {
                        std::string sample=m[0].str();
                        
                        std::string::size_type offset=element.second.find(sample);
                        if(offset==std::string::npos)
                        {
                            printf("error: if(offset==std::string::npos) \n");
                        }
                        {
                            report r;
                            r.value=sample;
                            r.lineNo=element.first;
                            r.offset=offset;
                            
                            /// mutex lock start
                            std::unique_lock<std::mutex> lock(mx_reports_mutex);
                            mx_reports.push_back(r);
                            /// mutex unlocks here
                        }
                    }
                }


            }

        }));
    }

    FILE * f=fopen(argv[1],"r");
    char buffer[1000];
    int cnt=0;
    while(auto ret=fgets(buffer,sizeof (buffer),f)!=NULL)
    {

        {
            std::unique_lock<std::mutex> lock(mx_strings_queue_mutex);
            mx_strings_queue.push_back({cnt,buffer});
            cond_var.notify_one();
            cnt++;
        }
    }

    while(true)
    {
        usleep(100000);
        {
            std::unique_lock<std::mutex> lock(mx_strings_queue_mutex);
            /// if container is empty - all done
            /// enable variable 'done' and signal threads to allow them be finished
            if(mx_strings_queue.empty())
            {
                done=true;
                cond_var.notify_all();
                break;
            }
            else
            {
        	cond_var.notify_all();
            }

        }
    }


    /// wait all threads termination
    for(auto& t:handlers)
    {
        t.join();
    }


    /// print report, mutex locking not needed
    printf("%s\n",std::to_string(mx_reports.size()).c_str());
    for(auto& r:mx_reports)
    {
        printf("%s %s %s\n",
               std::to_string(r.lineNo+1).c_str(),
               std::to_string(r.offset+1).c_str(),
               r.value.c_str()
               );
    }


}
