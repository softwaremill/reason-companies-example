const HtmlPlugin = require('webpack-html-plugin')
const join = require('path').join
const ExtractTextPlugin = require("extract-text-webpack-plugin");

const stats = {
    chunks: false,
    modules: false,
}

module.exports = [{
    entry: './lib/js/src/client',
    output: {
        filename: 'app.js',
        path: join(__dirname, 'bundle'),
        publicPath: '/',
    },
    plugins: [
        new HtmlPlugin({
            template: './index.html',
        }),
        new ExtractTextPlugin("styles.css")
    ],
    module: {
        rules: [{
            test: /\.css$/,
            use: ExtractTextPlugin.extract({
                fallback: "style-loader",
                use: "css-loader"
            })
        }],
    },
    stats,
    devServer: {
        stats
    }
}]