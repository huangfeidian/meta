public:
bool rpc_call(std::uint16_t rpc_index, const std::string& args)
{
    json json_arg;
    auto& the_logger = meta::utils::get_logger();
    if(args.size())
    {
        json_arg = json::parse(args);
    }
    switch(rpc_index)
    {
        case 0:
        {
            int temp_0;
            std::basic_string<char,std::char_traits<char>,std::allocator<char>> temp_1;
            bool decode_result = decode_multi(json_arg, temp_0, temp_1);
            if(decode_result)
            {
                rpc_1(temp_0, temp_1);
            }
            else
            {
                the_logger.warn("fail to decode args {} for rpc rpc_1 index {}", args,  cur_rpc_index);
            }
            return decode_result;
        }
        case 1:
        {
            
            bool decode_result = decode_multi(json_arg);
            if(decode_result)
            {
                rpc_2();
            }
            else
            {
                the_logger.warn("fail to decode args {} for rpc rpc_2 index {}", args,  cur_rpc_index);
            }
            return decode_result;
        }
        default:
            the_logger.warn("cant find rpc_name with rpc_index {} args {}", rpc_index, args);
            return false;
    }
	
}

static const std::unordered_map<std::string, std::uint16_t>& rpc_indexes()
{
    static std::unordered_map<std::string, std:uint16_t> rpc_index_map = {
        { "rpc_1", 0 },
        { "rpc_2", 1 }
    }
    retrurn rpc_index_map;
}
private:
