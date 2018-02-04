/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SuffixTree.h
 * Author: alexander
 *
 * Created on February 4, 2018, 5:23 PM
 */

#ifndef SUFFIXTREE_H
#define SUFFIXTREE_H

#include <vector>
#include <utility>
#include <limits>
#include <unordered_map>

template <typename charT>
struct CharHashSetFinder
{
    size_t GetChild(charT c) const
    {
        const auto it = m_map.find(c);
        if(it == m_map.end())
            return std::numeric_limits<size_t>::max();
        else
            return (*it).second;
    }
    
    void SetChild(charT c, size_t iNode)
    {
        m_map[c] = iNode;
    }
    
    std::vector<size_t> GetChildren() const
    {
        std::vector<size_t> res;
        for(const std::pair<charT, size_t>& p : m_map)
            res.push_back(p.second);
        return res;
    }    
    
private:
    std::unordered_map<charT, size_t> m_map;
};

// first template parameter - the character type
// second parameter a type that will be instantiated in every tree node and will be
// responsible for checking if there is an edge representing a given character.
// (essentially, a map/dictionary). There are two general approaches to this:
// (a) have an array whose size is equal to the size of the alphabet and
// (b) use a hash-map
template <typename charT, class TCharToSizeTMap> class SuffixTree {
public:
    static size_t Infinity() { return std::numeric_limits<size_t>::max(); };

    struct sNode {
        friend class SuffixTree;
        
        template <typename... Args>
        sNode(size_t in_iStart, size_t in_length, Args&&... args)
        : iStart(in_iStart)
        , length(in_length)
        , m_children(std::forward<Args>(args)...) {
            suffixLink = Infinity(); //this should help find bugs
        }

        size_t GetChild(charT c) const { return m_children.GetChild(c); }
        
        void SetChild(charT c, size_t iNode) { m_children.SetChild(c, iNode); }
        
        std::vector<size_t> GetChildren() const { return m_children.GetChildren(); }
        
        size_t GetStart() const { return iStart; }
        size_t GetLength() const { return length; }

    private:
        size_t iStart;
        size_t length;
        size_t suffixLink;
        TCharToSizeTMap m_children;
    };    
    

    template <typename... Args>
    SuffixTree(charT* str, size_t len, Args&&... args)
    {
        m_s.resize(len);
        std::copy(str, str+len, m_s.begin());
        m_v.emplace_back(0, 0, std::forward<Args>(args)...);

        size_t iNode = 0;
        size_t pos = 0;
        for( size_t i = 0; i < len; ++i )
            std::tie(iNode, pos) = AddChar(iNode, pos + 1, i+1, std::forward<Args>(args)...);
        
        AdjustLengths();
    }
    
    const sNode& GetNode(size_t i) const {
        return m_v[i];
    }

private:    
    template <typename... Args> 
    std::tuple<size_t, size_t> AddChar(size_t iNode, size_t pos, size_t curLen, Args&&... args)
    {
        size_t linkToSet = 0;		
        while(pos > 0)
        {
            charT c;			
            while(true)
            {
                c = m_s[curLen - pos];
                size_t iChild = m_v[iNode].GetChild(c);
                if( iChild == Infinity() || pos <= m_v[iChild].length)
                    break;                
                pos -= m_v[iChild].length;
                iNode = iChild;
            }
            
            size_t iChild = m_v[iNode].GetChild(c);
            if( iChild == Infinity() )
            {
                if(linkToSet != 0)
                    m_v[linkToSet].suffixLink = iNode;
                linkToSet = 0;
                m_v[iNode].SetChild(m_s[curLen - pos], m_v.size());
                m_v.emplace_back(curLen - pos, Infinity(), std::forward<Args>(args)...);                
            }
            else
            {
                charT cNeed = m_s[curLen-1];
                charT cGot = m_s[m_v[iChild].iStart + pos-1];
                if( cNeed == cGot )
                {
                    if(linkToSet != 0)
                    m_v[linkToSet].suffixLink = iNode;
                    break;
                }				
                else
                {
                    m_v[iNode].SetChild(c, m_v.size());
                    m_v.emplace_back( m_v[iChild].iStart, pos - 1, std::forward<Args>(args)... );
                    m_v[iChild].iStart += (pos - 1);
                    m_v[iChild].length -= (pos - 1);
                    m_v.back().SetChild(cGot, iChild);
                    m_v.back().SetChild(cNeed, m_v.size());
                    m_v.emplace_back(curLen - 1, Infinity(), std::forward<Args>(args)...);
                    if(linkToSet != 0)
                        m_v[linkToSet].suffixLink = m_v.size()-2;
                    linkToSet = m_v.size() - 2;
                }
            }            

            if( iNode == 0 )
                --pos;
            else
                iNode = m_v[iNode].suffixLink;
        }
        
        return std::tuple<size_t, size_t>( iNode, pos );
    }
    
    void AdjustLengths()
    {
        size_t charsLeft = m_s.size();
        size_t iNode = 0;
        for(size_t i = 0; i < m_s.size(); ++i)
        {
            size_t iChild;
            while(true)
            {                
                charT c = m_s[m_s.size() - charsLeft];
                iChild = m_v[iNode].GetChild(c);
                if( m_v[iChild].length > charsLeft )
                    break;
                
                charsLeft -= m_v[iChild].length;
                iNode = iChild;
            }
            
            m_v[iChild].length = charsLeft;
            if( iNode == 0 )
                --charsLeft;
            else
                iNode = m_v[iNode].suffixLink;
        }        
    }
    
    std::vector<sNode> m_v;
    std::vector<charT> m_s;
};

#endif /* SUFFIXTREE_H */

