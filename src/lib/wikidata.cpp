#include "wikidata.hpp"
#include "read_async.hpp"
#include "utils.hpp"
#include "stopwatch.hpp"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <map>

using namespace babelscript::console;
using namespace babelscript::network;
using boost::tokenizer;
using boost::escaped_list_separator;

class Wikidata::Impl
{
public:
  Impl(network::Babelscript* n, fs::path file_name)
    : _n(n)
    , _file_name(file_name)
  {
  }

  bool read_index_file();
  void write_index_file();
  fs::path index_file_name();

  network::Babelscript* _n{ nullptr };
  fs::path _file_name;
  std::map<std::string, size_t> _index;
  std::mutex _mtx;
  std::string _last_entry;
  size_t _last_index;
};

Wikidata::Wikidata(network::Babelscript* n, fs::path file_name)
  : _pImpl(new Impl(n, file_name))
{
}

Wikidata::~Wikidata()
{
  delete _pImpl;
}


void Wikidata::import_all()
{
  _pImpl->_n->print(L"Importing file " + _pImpl->_file_name.wstring(), true);

  ReadAsync read_async(_pImpl->_file_name);
  //std::wifstream stream(_pImpl->_file_name.string());

  std::vector<std::thread> threads;

  //for (std::wstring line; std::getline(stream, line); )
  std::wstring line;
  size_t streampos;
  while (read_async.get_line(line, streampos))
  {
    process_entry(line, streampos);
//    size_t i = 0;
//    for (i = 0; i < threads.size(); ++i)
//      if (!(_running & 1ull<<i))
//        break;

//    _running = _running | 1ull<<i;
//    std::thread t(&Wikidata::process_command, this, line, i);
//    if (i < threads.size())
//    {
//      if (threads[i].joinable()) threads[i].join();
//      threads[i] = std::move(t);
//    }
//    else
//    {
//      threads.emplace_back(std::move(t));
//      _pImpl->_n->print(L"Running " + std::to_wstring(threads.size()) + L" threads for reading wikidata file...", true);
//    }
  }

  for (std::thread& t : threads)
    if (t.joinable()) t.join();

  if (!read_async.error_text().empty()) throw std::runtime_error(read_async.error_text());
}


void Wikidata::process_entry(const std::wstring& line, size_t streampos, const size_t thread_index)
{
  static const std::wstring id_tag(L"\"id\":\"");
  size_t id0 = line.find(id_tag);

  if (id0 != std::wstring::npos)
  {
    size_t id1 = line.find(L"\"", id0 + id_tag.size() + 1);
    std::wstring id(line.substr(id0 + id_tag.size(), id1 - id0 - id_tag.size()));

    Node current = _pImpl->_n->node(id, "wikidata");

    size_t language0;
    static const std::wstring language_tag(L"{\"language\":\"");
    size_t labels = line.find(L"\"labels\":{");
    size_t aliases = line.find(L"\"aliases\":{", id1+7);
    size_t descriptions = line.find(L"\"descriptions\":{", id1+7);

    while ((language0 = line.find(language_tag, id1 + 7)) != std::wstring::npos
      && language0>labels
      && (aliases     ==std::wstring::npos || language0<aliases)
      && (descriptions==std::wstring::npos || language0<descriptions))
    {
      size_t language1 = line.find(L"\"", language0 + language_tag.size());
      std::string language = utils::str(line.substr(language0 + language_tag.size(), language1 - language0 - language_tag.size()));

      static const std::wstring value_tag(L"\"value\":\"");
      id0 = line.find(value_tag, language1 + 1);
      id1 = line.find(L"\"", id0 + value_tag.size() + 1);
      std::wstring value(line.substr(id0 + value_tag.size(), id1 - id0 - value_tag.size()));

      if (language == "en" || language == "de")
      {
        _pImpl->_n->set_name(current, value, language);
        _pImpl->_n->print(id + L": " + utils::wstr(language) + L": " + value, false);
      }
    }

    size_t property0;
    static const std::wstring property_tag(LR"(":[{"mainsnak":{"snaktype":"value","property":")");
    while ((property0 = line.find(property_tag, id1 + 1)) != std::wstring::npos)
    {
      size_t property1 = line.find(L"\"", property0 + property_tag.size());
      std::wstring property = line.substr(property0 + property_tag.size(), property1 - property0 - property_tag.size());

      static const std::wstring numeric_id_tag(LR"(","datavalue":{"value":{"entity-type":"item","numeric-id":)");

      if (line.substr(property1, numeric_id_tag.size()) == numeric_id_tag)
      {
        id0 = property1 + numeric_id_tag.size();

        bool success = true;
        while (line[++id0] != L',')
        {
          if (line[id0] < L'0' || line[id0]>L'9')
          {
            success = false;
            break;
          }
        }

        if (success)
        {
          static const std::wstring object_tag(LR"("id":")");
          if (line.substr(id0 + 1, object_tag.size()) == object_tag)
          {
            id0 += object_tag.size() + 1;
            id1 = line.find(L"\"", id0);
            std::wstring object = line.substr(id0, id1 - id0);
            //std::wcout << L" ---> " << property << L" " << object << std::endl;

            try
            {
              auto fact = _pImpl->_n->fact(current, _pImpl->_n->node(property, "wikidata"), { _pImpl->_n->node(object, "wikidata") });
              std::wstring output;
              _pImpl->_n->format_fact(output, "en", fact);
              _pImpl->_n->print(L"      en> " + output, false);
              _pImpl->_n->format_fact(output, "wikidata", fact);
              _pImpl->_n->print(L"wikidata> " + output, false);
            }
            catch (std::exception& ex)
            {
              _pImpl->_n->print(utils::wstr(ex.what()), true);
            }
          }
          else
          {
            id1 = id0 + object_tag.size() + 1;
          }
        }
        else
        {
          id1 = property1 + numeric_id_tag.size();
        }
      }
      else
      {
        id1 += property_tag.size();
      }
    }
  }

  _running = _running & ~(1ull << thread_index);
}



void Wikidata::generate_index()
{
  if (!_pImpl->read_index_file())
  {
    _pImpl->_n->print(L"Indexing file " + _pImpl->_file_name.wstring(), true);

    ReadAsync read_async(_pImpl->_file_name);
    //std::wifstream stream(_pImpl->_file_name.string());

    std::vector<std::thread> threads;

    StopWatch watch;
    watch.start();

    //for (std::wstring line; std::getline(stream, line); )
    std::wstring line;
    size_t streampos;
    while (read_async.get_line(line, streampos))
    {
      index_entry(line, streampos);
  //    size_t i = 0;
  //    for (i = 0; i < threads.size(); ++i)
  //      if (!(_running & 1ull<<i))
  //        break;

  //    _running = _running | 1ull<<i;
  //    std::thread t(&Wikidata::process_command, this, line, i);
  //    if (i < threads.size())
  //    {
  //      if (threads[i].joinable()) threads[i].join();
  //      threads[i] = std::move(t);
  //    }
  //    else
  //    {
  //      threads.emplace_back(std::move(t));
  //      _pImpl->_n->print(L"Running " + std::to_wstring(threads.size()) + L" threads for reading wikidata file...", true);
  //    }

      if (watch.duration() >= 1000)
      {
        {
          std::lock_guard<std::mutex> lock(_pImpl->_mtx);
          _pImpl->_n->print(L"Indexed " + std::to_wstring(_pImpl->_index.size()) + L" wikidata entries, latest is '" + utils::wstr(_pImpl->_last_entry) + L"' at position " + std::to_wstring(_pImpl->_last_index) + L" (" + std::to_wstring(_pImpl->_last_index/1024.0/1024/1024) + L" GB)", true);
        }
        watch.start();
      }
    }

    _pImpl->write_index_file();
  }
}


void Wikidata::index_entry(const std::wstring& line, const size_t streampos)
{
  static const std::wstring id_tag(L"\"id\":\"");
  size_t id0 = line.find(id_tag);

  if (id0 != std::wstring::npos)
  {
    size_t id1 = line.find(L"\"", id0 + id_tag.size() + 1);
    std::wstring id(line.substr(id0 + id_tag.size(), id1 - id0 - id_tag.size()));
    std::string id0 = utils::str(id);

    std::lock_guard<std::mutex> lock(_pImpl->_mtx);
    _pImpl->_index[id0] = streampos+id1;
    _pImpl->_last_entry = id0;
    _pImpl->_last_index = streampos+id1;
  }
}


void Wikidata::traverse(std::wstring start_entry, int max_depth)
{

}


bool Wikidata::Impl::read_index_file()
{
  if (!fs::exists(index_file_name()) || fs::last_write_time(_file_name) > fs::last_write_time(index_file_name()))
  {
      return false;
  }

  _n->print(L"Reading index file " + index_file_name().wstring() + L"...", true);
  std::ifstream stream(index_file_name());
  boost::archive::text_iarchive iarch(stream);
  iarch >> _index;
  _n->print(L"Finished reading", true);

  return true;
}

void Wikidata::Impl::write_index_file()
{
  _n->print(L"Writing index file " + index_file_name().wstring() + L"...", true);
  std::ofstream stream(index_file_name());
  boost::archive::text_oarchive oarch(stream);
  oarch << _index;
  _n->print(L"Finished writing", true);
}

fs::path Wikidata::Impl::index_file_name()
{
  return _file_name.parent_path() / (_file_name.stem().string() + ".index") ;
}
