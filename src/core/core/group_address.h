/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Microsoft Corporation
 * 
 * -=- Robust Distributed System Nucleus (rDSN) -=- 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
# pragma once

# include <dsn/cpp/address.h>
# include <algorithm> // for std::find()

namespace dsn
{
    class rpc_group_address
    {
    public:
        rpc_group_address(const char* name);
        bool add(rpc_address addr);
        void set_leader(rpc_address addr);
        bool remove(rpc_address addr);
        bool contains(rpc_address addr);

        dsn_group_t handle() const { return (dsn_group_t)this; }
        const std::vector<rpc_address>& members() const { return _members; }
        rpc_address random_member() const { return _members.empty() ? _invalid : _members[dsn_random32(0, (uint32_t)_members.size() - 1)]; }
        rpc_address next(rpc_address current) const;
        rpc_address leader() const { return _leader_index >= 0 ? _members[_leader_index] : _invalid; };
        void leader_forward();
        rpc_address possible_leader();
        const char* name() const { return _name.c_str(); }
        rpc_address address() const { return _group_address; }

    private:
        typedef std::vector<rpc_address> members_t;
        members_t   _members;
        int         _leader_index;
        std::string _name;
        rpc_address _group_address;
        static const rpc_address _invalid;
    };

    // ------------------ inline implementation --------------------

    inline rpc_group_address::rpc_group_address(const char* name)
    {
        _name = name;
        _leader_index = -1;
        _group_address.assign_group(handle());
    }

    inline bool rpc_group_address::add(rpc_address addr)
    {
        if (_members.end() == std::find(_members.begin(), _members.end(), addr))
        {
            _members.push_back(addr);
            return true;
        }
        else
            return false;
    }

    inline void rpc_group_address::leader_forward() {
        if (_members.empty()) return;
        _leader_index = (_leader_index+1)%_members.size();
    }

    inline void rpc_group_address::set_leader(rpc_address addr)
    {
        if (addr.is_invalid())
        {
            _leader_index = -1;
        }
        else
        {
            for (int i = 0; i < (int)_members.size(); i++)
            {
                if (_members[i] == addr)
                {
                    _leader_index = i;
                    return;
                }
            }

            _members.push_back(addr);
            _leader_index = (int)(_members.size() - 1);
        }
    }

    inline rpc_address rpc_group_address::possible_leader()
    {
        if (_leader_index == -1)
            return random_member();
        else
            return _members[_leader_index];
    }

    inline bool rpc_group_address::remove(rpc_address addr)
    {
        auto it = std::find(_members.begin(), _members.end(), addr);
        bool r = (it != _members.end());
        if (r)
        {
            if (-1 != _leader_index && addr == _members[_leader_index])
                _leader_index = -1;

            _members.erase(it);
        }
        return r;
    }

    inline bool rpc_group_address::contains(rpc_address addr)
    {
        return _members.end() != std::find(_members.begin(), _members.end(), addr);
    }

    inline rpc_address rpc_group_address::next(rpc_address current) const
    {
        if (current.is_invalid())
            return random_member();
        else
        {
            auto it = std::find(_members.begin(), _members.end(), current);
            if (it == _members.end())
                return random_member();
            else
            {
                it++;
                return it == _members.end() ? _members[0] : *it;
            }
        }
    }
}
