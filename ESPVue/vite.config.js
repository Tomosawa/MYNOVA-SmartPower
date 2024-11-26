// Plugins
import Components from 'unplugin-vue-components/vite'
import Vue from '@vitejs/plugin-vue'
import Vuetify, { transformAssetUrls } from 'vite-plugin-vuetify'
import ViteFonts from 'unplugin-fonts/vite'

// Utilities
import { defineConfig } from 'vite'
import { fileURLToPath, URL } from 'node:url'

// https://vitejs.dev/config/
export default defineConfig({
  base: '/',
  plugins: [
    Vue({
      template: { transformAssetUrls }
    }),
    // https://github.com/vuetifyjs/vuetify-loader/tree/master/packages/vite-plugin#readme
    Vuetify(),
    Components(),
    // ViteFonts({
    //   google: {
    //     families: [{
    //       name: 'Roboto',
    //       styles: 'wght@100;300;400;500;700;900',
    //     }],
    //   },
    // }),
  ],
  define: { 'process.env': {} },
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    },
    extensions: [
      '.js',
      '.json',
      '.jsx',
      '.mjs',
      '.ts',
      '.tsx',
      '.vue',
    ],
  },
  server: {
    host:'0.0.0.0',
    port: 3000,
    historyApiFallback: true,
    proxy: {
      '/api/': {
        target: 'http://10.0.10.1', //目标域名
        changeOrigin: true, //需要代理跨域
        rewrite: (path) => path.replace('/^\/api/', ''), //路径重写，把'/api'替换为''
      },
    },
  },
	build: {
	    rollupOptions: {
	      output: {
	        // 用于缩短静态资源的文件名
	        entryFileNames: '[hash].js',
	        chunkFileNames: '[hash].js',
	        assetFileNames: '[hash].[ext]',
	        // 使用更简洁的哈希长度
	        // entryFileNames: '[name].[hash:8].js',
	        // chunkFileNames: '[name].[hash:8].js',
	        // assetFileNames: '[name].[hash:8].[ext]',
	      },
	    },
	  },
})
