# MESI-Two-Level L1 cache protocol outline
# (+) -> Hit, (-) -> Miss
# Previously there was only one possible way of (+) in each request, 
# Now there is two possible ways of (+).
1. Start
2. CPU Issues Request (Read/Write)
   CPU Issue : ST (Store)
      - Check L1 Cache
      - If Hit: In D-cache : Inform Hit to CPU                                      (+)
      - If Hit: In I-cache : Replace                                                (-)
      - If Miss: Already in neighbour : Inform Neighbour-Hit to Local CPU           (+)
      - If Miss: Space available in Local : Get from L2                             (-)
      - If Miss: Space not available in Local : space available in Neighbour    
        - Store in Neighbour : Inform Hit to local CPU                              (+)
      - If Miss: Space not available in Local : space not available in Neighbour
        - Replace                                                                   (-)

   CPU Issue : LD (Load)
      - Check L1 Cache
      - If Hit: In D-cache : Inform Hit to CPU                                      (+)
      - If Hit: In I-cache : Replace                                                (-)
      - If Miss: Already in neighbour : Inform Neighbour-Hit to Local CPU           (+)
      - If Miss: Not in neighbour : Space available in Local : Get from L2          (-)
      - If Miss: Not in neighbour : Space not available in Local                
        - Replace                                                                   (-)
3. End

------------------------------------------------------------------------------------------------------------------------------
# In depth algorithm

1. ST (Store)
   - Local CPU
   - CPU request enqueued in mandatoryQueue
   - Check L1 cache -> is_valid(L1Dcache_entry) or is_valid(L1Icache_entry)
     - If Valid -> (hh_store_hit) -> Inform CPU
     - If inValid
       - Space available in neighbour :
         Event : N_Store, S_Store, E_Store, W_Store
               transition : {NP, I} -> IN
               action : n_issueNGETX :: (Store in neighbour) Sent via 'n_RequestL1Network_out'

   - Neighbour CPU
   - New neighbour request enqueued in 'n_RequestL1Network_in'
     - If request to Store data : 
       Event : N_ST_Data
             transition : {NP, I} -> IN
             actions : n_issueNGETXN : Store the data here and inform L2 cache
             Response : t_sendWBAck : Send store ack back to local CPU via 'n_ResponseL1Network_out'

   - Local CPU
   - New neighbour response enqueued in 'n_ResponseL1Network_in'
     - If Valid store : 
        Event : N_ST_Done
        Transition : IN -> M
        action : hhxn_store_hit : Inform CPU, store completed
     - If invalid store :
        Event : N_RT_BACK
        - Get from L2 cache or replace

2. LD (Load)
   - Local CPU
   - CPU request enqueued in mandatoryQueue
   - Check L1 cache -> is_valid(L1Dcache_entry) or is_valid(L1Icache_entry)
     - If Valid -> (h_load_hit) -> Inform CPU
     - If inValid
       - DataBlock available in neighbour :
         Event : N_Load, S_Load, E_Load, W_Load
               transition : {M, E, S, I, NP} -> IN
               action : n_issueNGETS :: (Load from neighbour) Sent request 'n_RequestL1Network_out'

   - Neighbour CPU
   - New neighbour request enqueued in 'n_RequestL1Network_in'
     - If request to Load data : 
       Event : N_Fwd_Data
             transition : IN -> M
             actions : n_sendDataToNeighbour :Send requested dataBlock to neighbour via 'n_ResponseL1Network_out'

   - Local CPU
   - New neighbour response enqueued in 'n_ResponseL1Network_in'
     - If Valid Load : 
        Event : N_Load_Seq
        Transition : IN -> M
        action : hx_load_hit : Inform CPU, load completed
     - If invalid Load :
        Event : N_RT_BACK
        - Get from L2 cache or replace


------------------------------------------------------------------------------------------------------------------------------
# New implementations
# State
IN, AccessPermission:Read_Write, desc="Neighbour state, block is in proccess of neighbour operations";

# Events 
N_Load, desc="Load from north";
S_Load, desc="Load from south";
E_Load, desc="Load from east";
W_Load, desc="Load from west";

N_Fwd_Data, desc="Send data to neighbour";
N_Load_Seq, desc="Load sequencer from neighbour";

N_SStore, desc="Store in north : already present in M state";
S_SStore, desc="Store in south : already present in M state";
E_SStore, desc="Store in east : already present in M state";
W_SStore, desc="Store in west : already present in M state";

N_Store, desc="Store in north";
S_Store, desc="Store in north";
E_Store, desc="Store in north";
W_Store, desc="Store in north";

N_ST_Data, desc="Store data from neighbour";
N_ST_Done, desc="Store completed";

N_RT_Data, desc="Return invalid state";
R_Load, desc="Load returned data";

Not_Vaid, desc="Not valid";

# Ports
out_port(n_RequestL1Network_out, RequestMsg, n_RequestFromL1Cache);
out_port(n_ResponseL1Network_out, ResponseMsg, n_ResponseFromL1Cache);

in_port(n_ResponseL1Network_in, ResponseMsg, n_ResponseToL1Cache, rank = 2)
in_port(n_RequestL1Network_in, RequestMsg, n_RequestToL1Cache, rank = 1) 

# Functions
Entry getNorthDataCacheEntry(Addr addr)
Entry getSouthDataCacheEntry(Addr addr)
Entry getEastDataCacheEntry(Addr addr)
Entry getWestDataCacheEntry(Addr addr)

int findMinimum(int a, int b, int c, int d)
int checkValidity(int a, Entry entry, int occ)
int checkInValidity(int a, Entry entry, int occ)

Event neighbourLoadData_event(Addr addr)
Event neighbourValidStoreData_event(Addr addr)
Event neighbourInValidStoreData_event(Addr addr)

Entry getNeighbour(Addr addr, MachineID mac)

# Actions
action(n_issueNGETS, "na", desc="Issue GETS North")
action(s_issueSGETS, "sa", desc="Issue GETS South")
action(e_issueEGETS, "ea", desc="Issue GETS East")
action(w_issueWGETS, "wa", desc="Issue GETS West")

action(n_sendDataToNeighbour, "nd", desc="send data to neighbour")
action(n_returnInvalidState, "ndr", desc="send data back to neighbour")

action(n_issueNGETX, "nax", desc="Issue GETX North")
action(s_issueSGETX, "sax", desc="Issue GETX South")
action(e_issueEGETX, "eax", desc="Issue GETX East")
action(w_issueWGETX, "wax", desc="Issue GETX West")

action(n_stallAndWaitRequestQueue, "\zr", desc="Stall and wait the L1 N_request queue") 
action(n_issueNGETXN, "bnx", desc="Issue GETX from L2")
action(uu_profileNDataMiss, "\udmn", desc="Profile the demand miss of neighbour")
action(t_sendWBAck, "t", desc="Send writeback ACK")

action(hhn_store_hit, "\hn", desc="Notify sequencer that north store completed.")
action(hhs_store_hit, "\hs", desc="Notify sequencer that south store completed.")
action(hhe_store_hit, "\he", desc="Notify sequencer that east store completed.")
action(hhw_store_hit, "\hw", desc="Notify sequencer that west store completed.")

action(n_popNRequestQueue, "ln", desc="Pop incoming request queue and profile the delay within this virtual network")
action(n_popNIncomingResponseQueue, "oN", desc="Pop Incoming Response queue and profile the delay within this virtual network") 

action(hhxn_store_hit, "\hxn", desc="Notify sequencer that store completed.")
action(n_issueRGETS, "ar", desc="Issue RGETS") 

# Transitions
transition({M,E,S}, N_Fwd_Data)
transition(IN, N_Load_Seq, M)
transition({M,E,S,NP,I}, N_Load_Seq)
transition({NP,I}, N_Store, IN)
transition({NP,I}, S_Store, IN)
transition({NP,I}, E_Store, IN)
transition({NP,I}, W_Store, IN)

transition(IN, N_ST_Data)
transition({NP,I}, N_ST_Data, IN)
transition(IN, Data_all_Acks, M)
transition({M,E,S,I,NP, IN}, N_ST_Done, M)
transition({E,M}, N_SStore, M)
transition({E,M}, S_SStore, M)
transition({E,M}, E_SStore, M)
transition({E,M}, W_SStore, M) 
transition({S,E,M,IS,IM,SM,IS_I,PF_IS_I,M_I,SINK_WB_ACK,PF_IS,PF_IM},N_RT_Data) 
transition({NP,I}, R_Load, IS)


