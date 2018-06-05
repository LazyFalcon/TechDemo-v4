#include "gl_core_4_5.hpp"
#include "PerfTimers.hpp"
#include "Logging.hpp"
#include <regex>
extern bool CLOG_SPECIAL_VALUE;
void TimeRecord::update(timeType dt){
    last = dt;
    max = std::max(max, dt);
    total += dt;
    if(CLOG_SPECIAL_VALUE) captured = dt;
    ++count;
}

CpuTimerScoped::CpuTimerScoped(const std::string &name) : name(name){
    timer.start();
}
CpuTimerScoped::CpuTimerScoped(const std::string &name, i32 line) : name(name + " #" + toString(line)){
    timer.start();
}
CpuTimerScoped::~CpuTimerScoped(){
    timer.end();
    if(printRecords) log(name, " executed in: ", timer.get()/1000.0, "ms");
    if(saveRecords) cpuRecords[name].update(timer.get());
}
void CpuTimerScoped::between(i32 line){
    timer.middle();
    log(name, line, " executed in: ", timer.getString(), "ms");
}
void CpuTimerScoped::writeToFile(){
    std::smatch m;
    std::regex re(R"(\b(.*) .*)");


    toFile("### CPU Measurements [ms]");
    toFile("|Name|Last|Max|Captured|Total|Count|Average|");
    toFile("|----|----|---|--------|-----|-----|-------|");
    for(auto &record : cpuRecords){
        toFile(
             "|", record.first
            ,"|", record.second.last/1000.0
            ,"|", record.second.max/1000.0
            ,"|", record.second.captured/1000.0
            ,"|", record.second.total/1000.0
            ,"|", record.second.count
            ,"|", double(record.second.total)/record.second.count/1000.0
            ,"|"
        );
    }
}

std::map<std::string, TimeRecord> CpuTimerScoped::cpuRecords;
bool CpuTimerScoped::printRecords {true};
bool CpuTimerScoped::saveRecords {true};

GpuTimerScoped::GpuTimerScoped(const std::string &function){
    if(not gpuRecords.count(function)){
        gpuRecords[function] = std::make_pair(freeTimerIds.front(), TimeRecord{});
        freeTimerIds.pop();
    }
    auto &timer = gpuRecords[function];
    u64 tmp;
    gl::GetQueryObjectui64v(timer.first, gl::QUERY_RESULT, &tmp);

    timer.second.update(tmp/1000);

    gl::BeginQuery(gl::TIME_ELAPSED, timer.first);
}
GpuTimerScoped::~GpuTimerScoped(){
    gl::EndQuery(gl::TIME_ELAPSED);
}
void GpuTimerScoped::init(){
    u32 queryCount = 50;
    u32 queries[queryCount];
    gl::GenQueries(queryCount, queries);
    for(u32 i = 0; i < queryCount; i++){
        gl::BeginQuery(gl::TIME_ELAPSED, queries[i]);
        gl::EndQuery(gl::TIME_ELAPSED);
        freeTimerIds.push(queries[i]);
    }
    int maxSize[4] = {0};
    for(u32 i = 0; i < queryCount; i++){
        gl::BeginQuery(gl::TIME_ELAPSED, queries[i]);
        gl::GetIntegerv(gl::MAX_TEXTURE_SIZE, maxSize);
        gl::GetIntegerv(gl::MAX_TEXTURE_SIZE, maxSize);
        gl::GetIntegerv(gl::MAX_TEXTURE_SIZE, maxSize);
        gl::GetIntegerv(gl::MAX_TEXTURE_SIZE, maxSize);
        gl::GetIntegerv(gl::MAX_TEXTURE_SIZE, maxSize);
        gl::EndQuery(gl::TIME_ELAPSED);
    }
}
void GpuTimerScoped::print(){
    return;
    clog("### GPU Measurements");
    u64 totalLast = 0;
    u64 totalAvg = 0;
    for(auto &it : gpuRecords){
        totalLast += it.second.second.last;
        totalAvg += it.second.second.total/it.second.second.count;
        clog(it.first, it.second.second.last/1000.f, "ms \t|", (it.second.second.total/it.second.second.count)/1000.f);
    }
    clog("Total", totalLast/1000.f, "ms \t|", totalAvg/1000.f, "ms");
    clog("---------------------------------------");
}
void GpuTimerScoped::writeToFile(){
    toFile("### GPU Measurements [ms]");
    toFile("|Name|Last|Max|Captured|Total|Count|Average|");
    toFile("|----|----|---|--------|-----|-----|-------|");
    TimeRecord sum {};
    double averageSum = 0;
    for(auto &record : gpuRecords){
        sum.last += record.second.second.last;
        sum.max += record.second.second.max;
        sum.captured += record.second.second.captured;
        sum.total += record.second.second.total;
        sum.count = std::max(sum.count, record.second.second.count);
        averageSum += double(record.second.second.total)/record.second.second.count/1000.0;
        toFile(
             "|", record.first
            ,"|", record.second.second.last/1000.0
            ,"|", record.second.second.max/1000.0
            ,"|", record.second.second.captured/1000.0
            ,"|", record.second.second.total/1000.0
            ,"|", record.second.second.count
            ,"|", double(record.second.second.total)/record.second.second.count/1000.0
            ,"|"
        );
    }
    toFile(
         "|Sum"
        ,"|", sum.last/1000.0
        ,"|", sum.max/1000.0
        ,"|", sum.captured/1000.0
        ,"|", sum.total/1000.0
        ,"|", sum.count
        ,"|", averageSum
        ,"|"
    );
    toFile("total/count", sum.total/sum.count/1000.0);
}

std::map<std::string, std::pair<u32, TimeRecord>> GpuTimerScoped::gpuRecords;
std::queue<u32> GpuTimerScoped::freeTimerIds;
