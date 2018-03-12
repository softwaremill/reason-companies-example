const HtmlPlugin = require('webpack-html-plugin')
const join = require('path').join
const ExtractTextPlugin = require("extract-text-webpack-plugin");

const stats = {
    chunks: false,
    modules: false,
}

const isProd = process.env.NODE_ENV === "production";

module.exports = [{
    entry: './lib/bs/client/index.bs.js',
    mode: isProd ? "production" : "development",
    output: {
        filename: '[name].js',
        path: join(__dirname, 'bundle'),
        publicPath: '/',
    },
    plugins: [
        new HtmlPlugin({
            template: './index.html',
            inject: true
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
    resolve: {
        extensions: ['.re', '.ml', '.js']
    },
    devServer: {
        stats
    }
}]