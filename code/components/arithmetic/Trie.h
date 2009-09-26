#pragma once

#include <string>
#include <map>
#include <cstdio>

template <typename T>
class Trie
{
    private:
        /// Maps the first letter of a string to a TrieLink
//         std::map<char, Trie<T>*> links;
        Trie<T>* links[256];
        
        /// Denotes that this link is a valid ending
        bool end;
        
        /// Any data that was stored at a word, only valid at 'end' nodes.
        T payload;
        
        void map_text_to_data(const char* text, size_t length, T& data)
        {
            /// Reached the end, set the payload
            if(length == 0)
            {
                end = true;
                payload = data;
                return;
            }
            
             /// Link has not yet been created, create a link containing the suffix of the string
            if(links[text[0]] == NULL)
            {
                Trie<T>* tmp = new Trie<T>();
                tmp->end = false;
                tmp->payload = data;
                
                links[text[0]] = tmp;
            }
            
            /// Add the remainder of the new link to the place
            links[text[0]]->map_text_to_data(text + 1, length - 1, data);
            
            return;
        }
        
        std::pair<bool, T*> contains_text(const char* text, size_t length)
        {
            /// Reached the end of the text
            if(length == 0)
            {
                if(end)
                    return std::pair<bool, T*>(end, &payload);
                else
                    return std::pair<bool, T*>(end, NULL);
            }
            
            /// Find the appropriate link
            Trie<T>* cur = links[text[0]];
            
            /// The appropriate link doesn't exist
            if(cur == NULL)
                return std::pair<bool, T*>(false, NULL);
            
            /// Carry on matching
            return cur->contains_text(text + 1, length - 1);
        }
        
        void delete_string(const char* text, size_t length)
        {
            /// Reached mapping to delete
            if(length == 0)
            {
                end = false;
                return;
            }
            
            /// Not finished, find the appropriate link
            Trie<T>* cur = links[text[0]];
            
            /// No link, stop
            if(cur == NULL)
                return;
            
            /// Link found, get the shared prefix length
            int prefix_length = cur->get_prefix(text, length);

            /// No total shared prefix, stop
            if(prefix_length != cur->link_text.length())
                return;
            
            /// Carry on deleting
            cur->link->delete_string(text + prefix_length, length - prefix_length);
        }
        
    public:
        Trie()
        {
            for(int i = 0; i < 256; ++i)
                links[i] = NULL;
        }
        
        void insert(std::string word, T data)
        {
            T temp = data;
            map_text_to_data(word.c_str(), word.size(), temp);
        }
        
        bool find_text(std::string word)
        {
            return contains_text(word.c_str(), word.size()).first;
        }
        
        T& find_data(std::string word)
        {
            return *contains_text(word.c_str(), word.size()).second;
        }
        
        std::pair<bool, T&> find(std::string word)
        {
            std::pair<bool, T*> tmp = contains_text(word.c_str(), word.size());
            
            return std::pair<bool, T&>(tmp.first, *(tmp.second));
        }
        
        void erase(std::string word)
        {
            delete_string(word.c_str(), word.size());
        }
        
        void destroy()
        {
            for(typename std::map<char, Trie<T>* >::iterator it = links.begin(); it != links.end(); ++it)
            {
                if(it->second != NULL)
                {
                    it->destroy();
                    delete it;
                }
            }
        }
};