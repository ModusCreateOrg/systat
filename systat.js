const os = require('os'),
      fs = require('fs'),
      si = require('systeminformation'),
      term = require('terminal-kit').terminal;

/*
 *console.log('term', term);
 *process.exit(1);
 */

const format = (n) => {
  return String(n.toLocaleString('en-US', { minimumFractionDigits: 2, maximumFractionDigits: 2 }));
};
const pct = (n) => {
  return String(parseInt(n*100+.5, 10)/100).padStart(5);
};
const kbyte = (n) => {
  n =  n / (1024);
  return n.toLocaleString().padStart(11);
};

let sleep = 1;

const updateVirtualMemory = () => {
  const lines = fs.readFileSync('/proc/vmstat').split('\n'),
        hash = {};
  for (const line of lines) {
    const [key, value] = line.split(' ');
    hash[key] = value;
  }
  return hash;
};
// poll status/info (update status obj)
const status = {};
const updateStatus = async () => {
  status.loadavg = os.loadavg();
  status.osinfo = await si.osInfo();
  status.currentLoad = await si.currentLoad();
  status.processes = await si.processes();
  status.cpu = await si.cpu();
  status.cpuCurrentSpeed = await si.cpuCurrentspeed();
  status.mem = await si.mem();
  status.fileSystems = await si.fsSize();
  status.diskIO = await si.disksIO();
  status.fsStats = await si.fsStats();
  status.if = await si.networkInterfaceDefault();
  status.interfaces = await si.networkInterfaces();
  status.network = await si.networkStats();
  if (fs.existsSync('/proc/vmstat')) {
    status.linux = true;
    status.vmstats = updateVirtualMemory();
  }
};


// output string at cursor, normal and inverse versions
const SPACES = new Array(1024).join(' ');

const normalLine = (row, col, s) => {
  const width = term.width;
  term.moveTo(row+1, col+1);
  term.inverse(false);
  term(s + SPACES.substr(0, width-s.length));
};
const inverseLine = (row, col, s) => {
  const width = term.width;
  term.moveTo(row+1, col+1);
  term.inverse(true);
  term(s + SPACES.substr(0, width-s.length));
  term.inverse(false);
};

const printHeader = (row) => {
  if (!status.osinfo) {
    return;
  }
  const info = status.osinfo,
        out = `systat/${sleep} ${info.hostname} [${info.distro}/${info.kernel}] ${new Date().toLocaleString()}`;
  inverseLine(0, row++, out);
  return row;
};

const printLoadAvg = (row) => {
  const loads = status.loadavg,
        procs = status.processes;
  normalLine(0,row++, `Load Average: ${format(loads[0])} ${format(loads[1])} ${format(loads[2])} Processes: ${procs.all} Running: ${procs.running}`);
  return row;
};

const printCPUs = (row) => {
  const load = status.currentLoad,
        cpu = status.cpu,
        cpus = load.cpus,
        num = cpus.length,
        header1 = `${cpu.manufacturer} ${cpu.brand} ${cpu.speed}GHz ${cpu.cores} cores`,
        header2 = ` ${'User'.padStart(6)} ${'System'.padStart(6)} ${'Nice'.padStart(6)} ${'Idle'.padStart(6)}`,
        hw = header1.length;

  inverseLine(0, row++, header1 + header2);
  normalLine(0, row++, `${'CPU'.padEnd(hw)} ${pct(load.currentload_user)}% ${pct(load.currentload_system)}% ${pct(load.currentload_nice)}% ${pct(load.currentload_idle)}%`);
  return row;
/*
 *  for (let row=0; row<num; row++) {
 *    const {load_user, load_system, load_nice, load_idle} = cpus[row],
 *          label = `CPU${row}`.padEnd(6);
 */

/*
 *    normalLine(0, 4+row, `${label} ${pct(load_user)}% ${pct(load_system)}% ${pct(load_nice)}% ${pct(load_idle)}%`);
 *  }
 */

};

const printMemory = (row) => {
  const mem = status.mem,
        total = kbyte(mem.total),
        free = kbyte(mem.free),
        used = kbyte(mem.used),
        active = kbyte(mem.active),
        avail = kbyte(mem.available);

  inverseLine(0, row++, `MEMORY         ${'Total'.padStart(12)} ${'Used'.padStart(12)} ${'Free'.padStart(12)} ${'Cache'.padStart(12)} ${'Avail'.padStart(12)}`);
  normalLine( 0, row++, `Real           ${total}K ${used}K ${free}K ${active}K ${avail}K`);

  const totalSwap = kbyte(mem.swaptotal),
        usedSwap = kbyte(mem.swapused),
        freeSwap = kbyte(mem.swapfree);

  normalLine( 0, row++, `Swap           ${totalSwap}K ${usedSwap}K ${freeSwap}K`);
  return row;
};

const printDisks = (row) => {
  let fileSystems = status.fileSystems,
      diskIO = status.diskIO,
      rio = format(diskIO.rIO_sec).padStart(15),
      wio = format(diskIO.wIO_sec).padStart(15),
      tio = format(diskIO.tIO_sec).padStart(15),
      fsStats = status.fsStats,
      rx = format(fsStats.rx_sec).padStart(15),
      wx = format(fsStats.wx_sec).padStart(15),
      tx = format(fsStats.tx_sec).padStart(15);

  inverseLine(0, row++, `DISK I/O ${'rIO/Sec'.padStart(15)} ${'wIO/sec'.padStart(15)} ${'tIO/sec'.padStart(15)} ${'rx/sec'.padStart(15)} ${'wx/sec'.padStart(15)} ${'tx/sec'.padStart(15)}`);
  normalLine( 0, row++, `         ${rio} ${wio} ${tio} ${rx} ${wx} ${tx}`);
  /*
   *  console.log(status.diskIO);
   *  console.log(status.fsStats);
   */

  row++;
  inverseLine(0, row++, `${'DISK'.padEnd(12)} ${'Size'.padStart(13)} ${'Used'.padStart(13)} ${'Capacity'.padStart(13)} Mount`);
  for (const fs of fileSystems) {
    const size = (fs.size/1024/1024/1024).toLocaleString().padStart(12),
          used = (fs.used/1024/1024/1024).toLocaleString().padStart(12),
          capacity = (fs.use).toLocaleString().padStart(12);
    normalLine(0, row++, `${fs.fs.padEnd(12)} ${size}G ${used}G ${capacity}% ${fs.mount}`);
  }
  return row;
};

const printNetwork = (row) => {
  const iface = status.network,
        name = iface.iface,
        rx = kbyte(iface.rx_sec),
        tx = kbyte(iface.rx_sec);

  inverseLine(0, row++, `${'NETWORK'.padEnd(16)} ${'RX/SEC'.padStart(17)} ${'TX/SEC'.padStart(17)}`);
  normalLine(0, row++, `${name.padEnd(16)} ${rx.padStart(16)}K ${tx.padStart(16)}K`);
  /*
   *  console.log(status.if);
   *  console.log(status.interfaces);
   */
  //  console.log(status.network);
  return row;
};
const main = () => {
  process.on('SIGINT', () => {
    term.attr('inverse', false);
    term.clear();
    term.hideCursor(false);
    process.exit(0);
  });
  term.hideCursor(true);
  term.clear();
  setInterval(async () => {
    await updateStatus();
    let row = 0;
    row = printHeader(row);
    row = printLoadAvg(row) + 1;
    row = printCPUs(row) + 1;
    row = printMemory(row) + 1;
    row = printDisks(row) + 1;
    row = printNetwork(row);
    /*
     *    console.log('status.currentLoad', status.currentLoad.cpus.length, status.currentLoad.cpus[0]);
     *    delete status.currentLoad.cpus;
     *    console.log('status.currentLoad', status.currentLoad.avgload);
     */
  }, 1000);
};

main();
