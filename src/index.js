import zip from './zip';
import fs from 'fs';
import path from 'path';
// import bin from './zip.wasm';

// const fs = require('fs'); 
import axios from 'axios';

const bin = fs.readFileSync(path.resolve(__dirname, 'zip.wasm'));

// console.log(fs.readFileSync(path.resolve(__dirname, 'zip.wasm')));

zip({wasmBinary: bin}).then(o =>{
    console.log(o._zip);
    console.log(o._unzip);
}).catch(e => console.log(e));

// // console.log(zip);

// zip()
//     .then(o => console.log(o))
//     .catch(e => console.log(e));

// console.log('hello world');