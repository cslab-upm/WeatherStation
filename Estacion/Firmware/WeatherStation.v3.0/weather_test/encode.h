struct timestamp;
struct weather;
void gen_packet(struct timestamp, struct weather, char* packet);
void gen_log(struct timestamp, struct weather, char* log);
