#if !defined(TEST_CLIPPERS_PROFILER_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

struct clock_record
{
    u64 Min;
    u64 Max;
    
    u32 MinIndex;
    u32 MaxIndex;

    u32 Count;
    u64 Average;
};

struct record_entry
{
    char *Key;

    clock_record Record;

    record_entry *Next;
};

struct record_hash_table
{
    record_entry **Entries;
    u32 Size;
};

#define TABLE_SIZE 4096
global_variable u32 CurrentOperationIndex = 0;
global_variable record_hash_table OperationTables[5][4] = {};

inline void
InitHashTables(void)
{
    for(u32 I = 0; I < 5; ++I)
    {
        for(u32 J = 0; J < 4; ++J)
        {
            record_hash_table *Table = &OperationTables[I][J];
            Table->Entries = (record_entry **)calloc(TABLE_SIZE, sizeof(record_entry *));
            Table->Size = 0;
        }
    }
}

inline u32
Hash(char *str)
{
    u32 hash = 0;
    s32 c = *str;
    while(c)
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
        c = *str++;
    }

    return hash;
}

inline char *
DublicateString(char *Source)
{
    u32 Size = 1;
    for(char *At = Source;
        *At;
        ++At)
    {
        ++Size;
    }

    char *Dest = (char *)malloc(Size);
    for(u32 CharIndex = 0;
        CharIndex < Size;
        ++CharIndex)
    {
        Dest[CharIndex] = Source[CharIndex];
    }

    Dest[Size - 1] = 0;    

    return(Dest);
}

inline record_entry *
CreateEntry(char *key, clock_record Record)
{
    record_entry *Entry = (record_entry *)malloc(sizeof(record_entry));
    Entry->Key = DublicateString(key);
    Entry->Record = Record;
    Entry->Next = 0;

    return(Entry);
}

inline void
Insert(record_hash_table *Table, char *key, clock_record Record)
{
    u32 Index = Hash(key) % TABLE_SIZE;
    record_entry *Entry = CreateEntry(key, Record); 
    Entry->Next = Table->Entries[Index];
    Table->Entries[Index] = Entry;
    Table->Size++;
}

inline b32
StringsAreEqual(char *A, char *B)
{
    b32 Result = (A == B);

    if(A && B)
    {
        while(*A && *B && (*A == *B))
        {
            ++A;
            ++B;
        }

        Result = ((*A == 0) && (*B == 0));
    }
    
    return(Result);
}

inline clock_record *
Get(record_hash_table *Table, char *key)
{
    clock_record *Result = 0;

    u32 Index = Hash(key) % TABLE_SIZE;
    record_entry *Entry = Table->Entries[Index]; 
    while(Entry)
    {
        if(StringsAreEqual(Entry->Key, key))
        {
            Result = &Entry->Record;
            break;
        }
    }

    return(Result);
}

struct record_block
{
    record_block(char *Key_, u32 ClipType_, u32 FillRule_)
    {
        Key = Key_;
        ClipType = ClipType_;
        FillRule = FillRule_;

        StartTSC = ReadCPUTimer();
    }
    
    ~record_block(void)
    {
        u64 Elapsed = ReadCPUTimer() - StartTSC;
        clock_record *Record = Get(&OperationTables[ClipType][FillRule], Key);

        if(Record->Min > Elapsed)
        {
            Record->Min = Elapsed;
            Record->MinIndex = CurrentOperationIndex;
        }

        if(Record->Max < Elapsed)
        {
            Record->Max = Elapsed;
            Record->MaxIndex = CurrentOperationIndex;
        }

        Record->Average += Elapsed;
        ++Record->Count;
    }
    
    char *Key;
    u32 ClipType;
    u32 FillRule;
    u64 StartTSC;
};

#define RecordBlock(Name, Clip, Fill) record_block NameConcat(Block, __LINE__)(Name, Clip, Fill);
#define RecordFunction(Clip, Fill) RecordBlock(__func__, Clip, Fill)

#define TEST_CLIPPERS_PROFILER_H
#endif
