#pragma once

#include "utils.h"

namespace network
{
  namespace utils
  {
    std::shared_ptr<Variables> join(const Variables& v1, const Variables& v2)
    {
      std::shared_ptr<Variables> result = std::make_shared<Variables>(v1);

      for (auto& var : v2)
      {
        auto it = result->find(var.first);
        
        if (it != result->end())
        {
          if (it->second != var.second)
          {
            throw std::runtime_error("Variable sets to be merged do conflict");
          }
        }
        else
        {
          (*result)[var.first] = var.second;
        }
      }

      return result;
    }
  }
}
