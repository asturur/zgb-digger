let diamondCounts = 0;
const lvlLetters = [
  "SHHHHHHHHHHHHHS",
  "VBCCCCBVCCCCCCV",
  "VCCCCCCV CCBC V",
  "V CCCC VCCBCCCV",
  "VCCCCCCV CCCC V",
  "V CCCC VBCCCCCV",
  "VCCBCCCV CCCC V",
  "V CCBC VCCCCCCV",
  "VCCCCCCVCCCCCCV",
  "HHHHHHHHHHHHHHH"]

const levelNumbers = lvlLetters.join('').split('')
    .map((char, i, arr) => {
    const digs =['V', 'H', 'S']
    if (char === 'C') {
      diamondCounts++;
      return ' 16,';
    }
    if (char === "B") {
      return ' 17,';
    }
    if (char === " ") {
      return '  0,';
    }
    let value = 0;
    const column = i % 15;
    const row =  (i - column ) / 15;
    if (column > 0 && digs.includes(arr[i-1])) {
      value += 1;
    }
    if (column < 14 && digs.includes(arr[i+1])) {
      value += 2;
    }
    if (row > 0 && digs.includes(arr[i-15])) {
      value += 8;
    }
    if (row < 10 && digs.includes(arr[i+15])) {
      value += 4;
    }
    return `${value.toString().padStart(3, " ")},`;
  });
  console.log(diamondCounts);

  const lines = new Array(10);
  lines.fill('');


  for ( let i = 0; i < 10; i++) {
    lines[i] = `${levelNumbers.slice(15 * i, 15 * (i + 1)).join('')} // ${lvlLetters[i]}`
  }

  const code =lines.join('\n');
  
  console.log(code);