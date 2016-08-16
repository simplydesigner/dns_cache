#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>

class dns_entry_t
{
public:
    dns_entry_t(const std::string & info) : m_info(info) {}
private:
    friend auto operator << (std::ostream & output, dns_entry_t const & entry) -> std::ostream &
    {
        output << entry.m_info;
        return output;
    }
    std::string m_info;
};

auto operator << (std::ostream & stream, std::shared_ptr<dns_entry_t> const & entry) -> std::ostream &
{
    if ( entry ) {
        return stream << *entry.get();
    }

    return stream;
}

template <typename ...args >
auto operator << (std::ostream & stream,  std::map<args...> const & map) -> std::ostream &
{
    for ( auto & entry : map ) {
        stream << "\t" << entry.first << ": " << entry.second << "\n";
    }
        
    return stream;
}

class dns_cache_t
{
public:
    auto find_entry(const std::string & domain) -> std::shared_ptr<dns_entry_t>
    {
        boost::shared_lock<boost::shared_mutex> lock { m_mutex };
        
        auto iterator = m_entries.find(domain);
        
        return iterator != m_entries.end() ? iterator->second : nullptr;
    }
    
    auto update_or_add_entry(const std::string & domain, const dns_entry_t & entry)
    {
        m_entries[domain] = std::make_shared<dns_entry_t>(entry);
    }
private:
    friend auto operator << (std::ostream & output, dns_cache_t const & cache) -> std::ostream &
    {
        return output << "cache(" << cache.m_entries.size() << " records): {\n" << cache.m_entries << "}\n";
    }
    mutable boost::shared_mutex m_mutex;
    std::map<std::string, std::shared_ptr<dns_entry_t>> m_entries;
};
        


int main(int argc, const char * argv[]) {

    dns_cache_t cache;
    std::cout << cache << std::endl;
    
    auto result = cache.find_entry("123");
    
    std::cout << cache << std::endl;
    dns_entry_t entry = {"123:456"};
    cache.update_or_add_entry("123", entry);
    result = cache.find_entry("123");
    
    std::cout << cache << std::endl;
    
    return 0;
}
