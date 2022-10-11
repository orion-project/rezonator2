const fs = require('fs');
const sharp = require('sharp');

const prefixTex = '.. tex:'
const prefixImg = '.. image::'

// SVG is too small by itself. It's supposed that mjx-container tag will scale it
// to match surrounding font (https://github.com/mathjax/MathJax/issues/2799)
// Since we don't have a 'surrounding font', we have to do some hardcoded scaling.
const svgScale = 1.5;

// Size of free space around formula
const pngMargin = 20;

const args = {
    dryRun: false,
    overwrite: false,
    inputFiles: [],
}

function addInputFile(fn) {
    if (fn.endsWith('.rst')) {
        if (!args.inputFiles.includes(fn)) {
            args.inputFiles.push(fn);
        }
    }
}

for (const arg of process.argv) {
    if (arg == '--dry-run') {
        args.dryRun = true;
    } else if (arg == '--overwrite') {
        args.overwrite = true;
    } else if (arg == '*') {
        for (const fn of fs.readdirSync('.')) {
            addInputFile(fn);
        }
    } else {
        addInputFile(arg);
    }
}

if (args.inputFiles.length == 0) {
    console.error('No input files')
    process.exit(1)
}

require('mathjax')
    .init({
        loader: {
            load: ['input/tex', 'output/svg'],
        },
    })
    .then(mathJax => {
        for (const fn of args.inputFiles) {
            processFile(fn, mathJax);
        }
    })
    .catch(err => {
        console.error(err);
        process.exit(1);
    });

function processFile(fn, mathJax) {
    console.log('****', fn);

    if (!fs.existsSync(fn)) {
        console.log('File not found');
        return;
    }

    const formulas = parseFile(fn);

    if (formulas.length == 0) {
        console.log('No formulas');
        return;
    }

    if (args.dryRun) {
        console.log('Formulas:', formulas.length);
        console.log(formulas);
        return;
    }

    for (const f of formulas) {
        if (fs.existsSync(f.img) && !args.overwrite) {
            console.log(f.img, 'already exists');
            continue;
        }
        tex2png(f, mathJax);
    }
}

function parseFile(fn) {
    const formulas = [];
    var formula = [];
    var lineNo = 0;
    var formulaLine = 0;
    for (const line of fs.readFileSync(fn, 'utf-8').split(/\r?\n/).map(l => l.trim())) {
        lineNo++;

        if (line.startsWith(prefixTex)) {
            formula = [line.substring(prefixTex.length).trim()];
            formulaLine = lineNo;
        } else if (line.startsWith(prefixImg)) {
            if (formula.length == 0) {
                continue;
            }
            formulas.push({
                line: formulaLine,
                tex: formula.join(' ').trim(),
                img: line.substring(prefixImg.length).trim(),
            });
            formula = [];
        } else {
            if (formula.length == 0) {
                continue;
            }
            formula.push(line);
        }
    }
    return formulas;
}

function tex2png(formula, mathJax) {
    const mjxNode = mathJax.tex2svg(formula.tex, {display: true});
    const svgNode = mjxNode.children[0];
    if (!svgNode || svgNode.kind != 'svg') {
        console.log('While rendering', formula);
        console.error('Invalid SVG node:', svgNode);
        return;
    }

    // Size here are in 'ex' units, parseFloat drops trailing non-digits
    const w = parseFloat(svgNode.attributes.width);
    const h = parseFloat(svgNode.attributes.height);
    if (isNaN(w) || isNaN(h)) {
        console.log('While rendering', formula);
        console.error('Invalid SVG sizes:', svgNode.attributes);
        return;
    }
    svgNode.attributes.width = `${w*svgScale}ex`;
    svgNode.attributes.height = `${h*svgScale}ex`;

    const svgText = mathJax.startup.adaptor.innerHTML(mjxNode);

    sharp(Buffer.from(svgText))
        .extend({
            left: pngMargin,
            top: pngMargin,
            right: pngMargin,
            bottom: pngMargin,
            background: {r: 0, g: 0, b: 0, alpha: 0},
        })
        .toFile(formula.img, (err, info) => {
            if (err) {
                console.log('While rendering', formula);
                console.error('Failed to save PNG:', err);
            } else {
                console.info(formula.img, 'saved:', info);
            }
        });
}
